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
KD11_NA_Cpu::KD11_NA_Cpu (Qbus* bus)
    :
    KD11CpuData (bus),
    runState {CpuRunState::HALT},
    kd11_naInstruction {},
    haltReason_ {HaltReason::HaltInstruction}
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
    // Generate a Trace trap if the trace bit is set, unless the previous
    // instruction was a RTT or another trap is pending.
    if (runState == CpuRunState::INHIBIT_TRACE)
    {
        runState = CpuRunState::RUN;
        bus_->SRUN().set (true);
    }
    else if (!trap_ && (psw_ & PSW_T))
    {
        trace.trap (TrapRecordType::TRAP_T, 014);
        setTrap (&traceTrap);
    }
    handleTraps ();

    // The continutaion of the step depends on the run state:
    // - HALT: the CPU is halted and cannot execute the step, return false,
	// - RUN: the CPU will execute the next instruction,
	// - WAIT: the CPU is running but is still waiting for an interrupt, return true
	// - INHIBIT_TRACE: the CPU is running but no trace trap has to be
    //   executed on this instruction. At this point in the step the cpu cannot
    //   be in this state.
    if (runState == CpuRunState::HALT)
        return false;

    if (runState == CpuRunState::WAIT)
        return true;

    if(trace.isActive ())
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

    // If there is a pending bus interrupt that can be executed, process
    // that interrupt first, else execute the next instruction
    if (!bus_->intrptReqAvailable () || cpuPriority () >= bus_->intrptPriority ())
    {
        std::chrono::high_resolution_clock::time_point start =
            std::chrono::high_resolution_clock::now ();
        execInstr ();
        std::chrono::high_resolution_clock::time_point end =
            std::chrono::high_resolution_clock::now ();
        trace.duration ("Instruction",
            (std::chrono::duration_cast<std::chrono::nanoseconds> (end - start)).count ());
    }

    return (runState != CpuRunState::HALT);
}

// Execute one instruction
void KD11_NA_Cpu::execInstr ()
{
    // Get next instruction to execute and move PC forward
    CondData<u16> instructionWord = fetchWord (registers_[7]);
    if (!instructionWord.hasValue())
    {
        trace.bus (BusRecordType::ReadFail, registers_[7], 0);
        setTrap (&busError);
        return;
    }
    registers_[7] += 2;

    unique_ptr<LSI11Instruction> instr = kd11_naInstruction.decode (this, instructionWord);
    CpuData::Trap returnedTrap = instr->execute ();
    if (returnedTrap != CpuData::Trap::None)
        setTrap (vectorTable [returnedTrap]);
}


// This function is called on every KD11_NA step, whether or not a trap to
// be handled is present. If a trap is present the current PC and PSW are
// saved on the stack and the PC and PSW of the trap vector are loaded.
// If there is no trap to be handled the function simply returns.
//
// Trap priority order from high to low (cf Fig. 2-13) is defined as (vectors
// between brackets):
// - Bus error (004)
// - Machine trap (BPT (014), IOT (020), EMT (030), TRAP (034) instruction)
// - Trace trap (PSW bit 4) (014)
// - Powerfail/HALT interrupt (024)
// - Event interrupt (LTC) (100)
// - Device interrupt
// 
// The event and device interrupts are only processed if the PSW priority bit
// is cleared.
// 
// Refer to the LSI-11 WCS user's guide (EK-KUV11-TM-001) par 2.3.
//

void KD11_NA_Cpu::handleTraps ()
{
    InterruptRequest intrptReq;
    u16 trapToProcess{ 0 };
    CondData<u16> tmpValue;

    // Traps are handled in order of their priority:
    // - Bus errors,
    // - Instruction traps
    // - Event and device interrupts, only if the priority bit is clear,
    // 
    // Interrupts are only processed if their priority is higher than the
    // current CPU priority. (The LSI-11 has just two priority levels,
    // zero and BR4.) Note that the numerical value of the TrapPriority enum
    // is used as bus request level. Traps in HALT mode are ignored.
    // 
    // Check if there is a trap or interrupt request to handle and the CPU
    // isn't halted. This is the most common case so check this as first.
    if ((!trap_ && !bus_->intrptReqAvailable ()) || runState == CpuRunState::HALT)
        return;

    // Traps have the highest priority, so first check if there is a trap
    // to handle
    if (trap_)
    {
        trapToProcess = trap_->vector ();
        trap_ = nullptr;
    }
    else if (bus_->intrptPriority () > cpuPriority ())
    {
        if (bus_->getIntrptReq (intrptReq))
            trapToProcess = intrptReq.vector ();
        else
            return;
    }
    else return;

    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, trapToProcess);

    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (trapToProcess);

    /* resume execution if in WAIT state */
    if (runState == CpuRunState::WAIT)
    {
        trace.cpuEvent (CpuEventRecordType::CPU_RUN, registers_[7]);
        runState = CpuRunState::RUN;
        bus_->SRUN().set (true);
    }
}

// Load PC and PSW from the given vector
void KD11_NA_Cpu::loadTrapVector (InterruptRequest const* trap)
{
    unsigned char trapVector = trap->vector ();
    registers_[7] = fetchWord (trapVector).valueOr (0);
    psw_ = fetchWord (trapVector + 2).valueOr (0);
}

u8 KD11_NA_Cpu::cpuPriority()
{
    return (psw_ & PSW_PRIORITY) >> 5;
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
u16 KD11_NA_Cpu::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = fetchWord (address);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

// Swap the PC and PSW with new values from the given vector
void KD11_NA_Cpu::swapPcPSW (u16 vecrorAddress)
{
    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!pushWord (psw_) || !pushWord (registers_[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, registers_[6]);
        // ToDo: All interrupts should be cleared?
        trap_ = nullptr;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::DoubleBusError;
        bus_->SRUN().set (false);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    if (!fetchFromVector (vecrorAddress, &registers_[7]) ||
        !fetchFromVector (vecrorAddress + 2, &psw_))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, vecrorAddress);
        trap_ = nullptr;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN().set (false);
        return;
    }
}
