#include "kd11_na_cpucontrol.h"
#include "kd/common/instructiondecoder/instructiondecoder.h"
#include "kd/kd11_na/executor/executor.h"
#include "kd/kd11_na/calculate/calculate.h"
#include "bus/qbus/qbus.h"
#include "pdp11peripheral/pdp11peripheral.h"
#include "float/float.h"
#include "trace/trace.h"
#include "bitmask.h"

#include <functional>
#include <chrono>
#include <variant>

using std::visit;
using std::chrono::microseconds;
using std::runtime_error;

// Constructor
KD11_NA_CpuControl::KD11_NA_CpuControl (Bus* bus, CpuData* cpuData, MMU* mmu)
    :
    bus_ {bus},
    mmu_ {mmu},
    cpuData_ {cpuData},
    runState {CpuRunState::HALT},
    decoder {},
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
// receivers which then calls a KD11_NA_CpuControl control function.
//
// This function will return true if the CPU is in the state RUN and another
// instruction can be executed, false otherwise. In the latter case a HALT
// instruction was executed.
CpuControl::CpuRunState KD11_NA_CpuControl::step ()
{
    /*
    switch (runState)
    {
        case CpuRunState::HALT:
            return false;

        case CpuRunState::WAIT:
            // If an interrupt request is present resume execution else
            // advance time so devices are awakened at the specified time.
            // ToDo: load trap vector at this point?
            if (bus_->intrptReqAvailable ())
            {
                trace.cpuEvent (CpuEventRecordType::CPU_RUN, cpuData_->registers ()[7]);
                runState = CpuRunState::RUN;
                bus_->SRUN().set (true);
            }
            else
                SimulatorClock::forwardClock (microseconds (50));
            return true;

        case CpuRunState::RUN:
            execute ();
            return runState != CpuRunState::HALT; 

        default:
            // Satisfy the compiler
            throw runtime_error ("Cannot happen");
    }
    */

    execute ();
    return runState;

}

void KD11_NA_CpuControl::execute ()
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
void KD11_NA_CpuControl::execInstr ()
{
    // Create an Calculate and Executor to time and execute the instructions
    KD11_NA::Calculate calculator {};
    KD11_NA::Executor executor (cpuData_, this, mmu_);

    // Get next instruction to execute and move PC forward
    CondData<u16> instructionWord = mmu_->fetchWord (cpuData_->registers ()[7]);
    if (!instructionWord.hasValue())
    {
        trace.bus (BusRecordType::ReadFail, cpuData_->registers ()[7], 0);
        cpuData_->setTrap (CpuData::TrapCondition::BusError);
        return;
    }
    cpuData_->registers ()[7] += 2;

    Instruction instr = 
        decoder.decode (instructionWord);

    // If the trace flag is set, the next instruction has to result in a trace
    // trap, unless the instruction resulted in another trap.
    if (traceFlag_)
        cpuData_->setTrap (CpuData::TrapCondition::BreakpointTrap);

    // The instruction time is defined in microseconds with an accuracy of
    // nanoseconds. Convert the time in microseconds to the 64-bits integer
    // number of nanoseconds.
    double instrTime = visit (calculator, instr);
    SimulatorClock::forwardClock
    (
        SimulatorClock::duration (static_cast<uint64_t> (instrTime * 1000))
    );

    // Execute the next instruction. The function returns true if the
    // instruction was completed and false if it was aborted due to an error
    // condition. In that case a trap has been set. Note however that trap
    // instructions set a trap and return true. 
    visit (executor, instr);

    if (cpuData_->trap () != CpuData::TrapCondition::None)
        serviceTrap ();

    // Trace Trap is enabled by bit 4 of the PSW and causes processor traps at
    // the end of instruction execution. The instruction-that is executed
    // after the instruction that set the T-bit will proceed to completion and
    // then trap through the trap vector at address 14.
    // LSI-11/PDP-11/03 Processor Handbook pag. 114.
    traceFlag_ =  (cpuData_->psw().traceBitSet ()) ? true : false;
} 

void KD11_NA_CpuControl::serviceTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (cpuData_->trapVector ());
    cpuData_->clearTrap ();
}

void KD11_NA_CpuControl::serviceInterrupt ()
{
    InterruptRequest intrptReq;
 
    if (bus_->getIntrptReq (intrptReq))
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());
}

u8 KD11_NA_CpuControl::cpuPriority()
{
    return cpuData_->psw ().priorityLevel ();
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
bool KD11_NA_CpuControl::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

bool KD11_NA_CpuControl::fetchFromVector (u16 address, function<void (u16)> lambda)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    lambda (tmpValue.valueOr (0));
    return tmpValue.hasValue ();
}

// Swap the PC and PSW with new values from the given vector
void KD11_NA_CpuControl::swapPcPSW (u16 vectorAddress)
{
    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, vectorAddress);

    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!mmu_->pushWord (cpuData_->psw ()) || !mmu_->pushWord (cpuData_->registers ()[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, cpuData_->registers ()[6]);
        // ToDo: All interrupts should be cleared?
        cpuData_->clearTrap ();
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::DoubleBusError;
        bus_->SRUN().set (false);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    if (!fetchFromVector (vectorAddress, &cpuData_->registers ()[7]) ||
        !fetchFromVector (vectorAddress + 2, [this] (u16 value) 
            {cpuData_->psw ().set (PSW::ProtectionMode::Trap, value);}))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, vectorAddress);
        cpuData_->clearTrap ();
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN().set (false);
        return;
    }
}

void KD11_NA_CpuControl::traceStep ()
{
    trace.setIgnoreBus ();
    u16 code[3];
    // The read of register_[7]+2 and  register_[7]+4 may access an invalid address as
    // the instruction isn't decoded at this point. Therefore use the bus
    // read function instead of fetchWord(). The latter will generate a
    // bus error trap on access of an invalid address.
    code[0] = mmu_->readWithoutTrap (cpuData_->registers ()[7] + 0).valueOr (0);
    code[1] = mmu_->readWithoutTrap (cpuData_->registers ()[7] + 2).valueOr (0);
    code[2] = mmu_->readWithoutTrap (cpuData_->registers ()[7] + 4).valueOr (0);
    trace.cpuStep (cpuData_->registers (), cpuData_->psw (), code);
    trace.clearIgnoreBus ();
}

