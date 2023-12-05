#include "kd11_na_cpu.h"
#include "qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "float/float.h"
#include "trace/trace.h"
#include "bitmask.h"

#include <functional>
#include <chrono>
#include <memory>

using std::unique_ptr;
using std::make_unique;

// Constructor
KD11_NA_Cpu::KD11_NA_Cpu (Qbus* bus, MMU* mmu)
    :
    KD11CpuData (bus),
    mmu_ {mmu},
    runState {CpuRunState::HALT},
    kd11_naInstruction {},
    haltReason_ {HaltReason::HaltInstruction},
    traceFlag_ {false}
{
    bus_->SRUN().set (false);
}

// Perform a CPU step. The step mainly comprises three actions:
// 1. Execution of an instruction,
// 2. Handle the trace bit,
// 3. Handling of traps and interrupts that might have arisen during execution
//    of the instruction, either a trap as a result of an instruction, or an
//    interrupt requested by a bus device.
// 
// The cpu always is in of the following states:
// - HALT: the CPU is halted and cannot execute the step, return false,
// - RUN: the CPU will execute the next instruction,
// - WAIT: the CPU is running but is still waiting for an interrupt, return true.
// 
// As the power-up mode can be set to trap to the vector at address 024, the
// presence of traps is checked before an instruction is executed.
//
// The normal instruction flow can be interrupted by the setting of the BHALT
// or RESET bus signal. These signals are handled in their corresponding KD11_NA
// receivers which then calls a KD11_NA_Cpu control function.
//
// This function will return true if the CPU is in the state RUN and another
// instruction can be executed, false otherwise. In the latter case a HALT
// instruction was executed.
bool KD11_NA_Cpu::step ()
{
    switch (runState)
    {
        case CpuRunState::HALT:
            return false;

        case CpuRunState::WAIT:
            // If an interrupt request is present resume execution
            // ToDo: load trap vector at this point?
            if (bus_->intrptReqAvailable ())
            {
                trace.cpuEvent (CpuEventRecordType::CPU_RUN, registers_[7]);
                runState = CpuRunState::RUN;
                bus_->SRUN().set (true);
            }
            return true;

        case CpuRunState::RUN:
            execute ();
            return runState != CpuRunState::HALT; 

        default:
            // Satisfy the compiler
            throw ("Cannot happen");
    }
}

void KD11_NA_Cpu::execute ()
{
    // If there is a pending bus interrupt that can be executed, process
    // that interrupt first, else execute the next instruction
    // Interrupts are only processed if their priority is higher than the
    // current CPU priority. (The LSI-11 has just two priority levels,
    // zero and BR4.) Note that the numerical value of the TrapPriority enum
    // is used as bus request level. Traps in HALT mode are ignored.
    if (bus_->intrptReqAvailable () && bus_->intrptPriority () > cpuPriority ())
        serviceInterrupt ();

    if(trace.isActive ())
        traceStep ();

    execInstr ();

    // Instructions leave the run state unchanged except for the WAIT
    // instcution which sets the state to WAIT.
}

// Execute one instruction
void KD11_NA_Cpu::execInstr ()
{
    // Get next instruction to execute and move PC forward
    CondData<u16> instructionWord = mmu_->fetchWord (registers_[7]);
    if (!instructionWord.hasValue())
    {
        trace.bus (BusRecordType::ReadFail, registers_[7], 0);
        setTrap (CpuData::TrapCondition::BusError);
        return;
    }
    registers_[7] += 2;

    unique_ptr<LSI11Instruction> instr = 
        kd11_naInstruction.decode (this, this, mmu_, instructionWord);

    // If the trace flag is set, the next instruction has to result in a trace
    // trap, unless the instruction resulted in another trap.
    if (traceFlag_)
        setTrap (CpuData::TrapCondition::BreakpointTrap);

    // Execute the next instruction. The function returns true if the
    // instruction was completed and false if it was aborted due to an error
    // condition. In that case a trap has been set. Note however that trap
    // instructions set a trap and return true. 
    instr->execute ();

    if (trap_ != CpuData::TrapCondition::None)
        serviceTrap ();

    // Trace Trap is enabled by bit 4 of the PSW and causes processor traps at
    // the end of instruction execution. The instruction-that is executed
    // after the instruction that set the T-bit will proceed to completion and
    // then trap through the trap vector at address 14.
    // LSI-11/PDP-11/03 Processor Handbook pag. 114.
    traceFlag_ =  (psw_ & PSW_T) ? true : false;
} 

void KD11_NA_Cpu::serviceTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (trapVector (trap_));
    trap_ = CpuData::TrapCondition::None;
}

void KD11_NA_Cpu::serviceInterrupt ()
{
    InterruptRequest intrptReq;
 
    if (bus_->getIntrptReq (intrptReq))
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());
}

u8 KD11_NA_Cpu::cpuPriority()
{
    return (psw_ & PSW_PRIORITY) >> 5;
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
u16 KD11_NA_Cpu::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

// Swap the PC and PSW with new values from the given vector
void KD11_NA_Cpu::swapPcPSW (u16 vectorAddress)
{
    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, vectorAddress);

    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!mmu_->pushWord (psw_) || !mmu_->pushWord (registers_[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, registers_[6]);
        // ToDo: All interrupts should be cleared?
        trap_ = CpuData::TrapCondition::None;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::DoubleBusError;
        bus_->SRUN().set (false);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    if (!fetchFromVector (vectorAddress, &registers_[7]) ||
        !fetchFromVector (vectorAddress + 2, &psw_))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, vectorAddress);
        trap_ = CpuData::TrapCondition::None;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN().set (false);
        return;
    }
}

void KD11_NA_Cpu::traceStep ()
{
    trace.setIgnoreBus ();
    u16 code[3];
    // The read of register_[7]+2 and  register_[7]+4 may access an invalid address as
    // the instruction isn't decoded at this point. Therefore use the bus
    // read function instead of fetchWord(). The latter will generate a
    // bus error trap on access of an invalid address.
    code[0] = bus_->read (registers_[7] + 0).valueOr (0);
    code[1] = bus_->read (registers_[7] + 2).valueOr (0);
    code[2] = bus_->read (registers_[7] + 4).valueOr (0);
    trace.cpuStep (registers (), psw_, code);
    trace.clearIgnoreBus ();
}

// The KD11-NA does not support a stack limit so stack overflow cannot occur.
bool KD11_NA_Cpu::stackOverflow ()
{
    return false;
}
