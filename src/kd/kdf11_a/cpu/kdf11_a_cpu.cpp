#include "kdf11_a_cpu.h"
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
KDF11_A_Cpu::KDF11_A_Cpu (Qbus* bus)
    :
    KD11CpuData (bus),
    runState {CpuRunState::HALT},
    kdf11_aInstruction {},
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
// The cpu always is in of the following states:
// - HALT: the CPU is halted and cannot execute the step, return false,
// - RUN: the CPU will execute the next instruction,
// - WAIT: the CPU is running but is still waiting for an interrupt, return true
// - INHIBIT_TRACE: the CPU is running but no trace trap has to be
//   executed on this instruction.
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
bool KDF11_A_Cpu::step ()
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

        case CpuRunState::INHIBIT_TRACE:
            trace.cpuEvent (CpuEventRecordType::CPU_RUN, registers_[7]);
            runState = CpuRunState::RUN;
            bus_->SRUN().set (true);
            execute ();
            return runState != CpuRunState::HALT;

        case CpuRunState::RUN:
            // Generate a Trace trap if the trace bit is set, unless the previous
            // instruction was a RTT or another trap is pending.
            if (!trap_ && (psw_ & PSW_T))
                setTrap (CpuData::Trap::BreakpointTrap);
            
            execute ();
            return runState != CpuRunState::HALT; 

        default:
            // Satisfy the compiler
            throw ("Cannot happen");
    }
}

void KDF11_A_Cpu::execute ()
{
    handleTrapsAndInterrupts ();

    if(trace.isActive ())
        traceStep ();

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

    // Instructions leave the run state unchanged except for:
    // - the RTT instruction sets the state to INHIBIT_TRACE,
    // - the WAIT instrcution sets the state to WAIT.
}

// Execute one instruction
void KDF11_A_Cpu::execInstr ()
{
    // Get next instruction to execute and move PC forward
    CondData<u16> instructionWord = fetchWord (registers_[7]);
    if (!instructionWord.hasValue())
    {
        trace.bus (BusRecordType::ReadFail, registers_[7], 0);
        setTrap (CpuData::Trap::BusError);
        return;
    }
    registers_[7] += 2;

    unique_ptr<LSI11Instruction> instr = 
        kdf11_aInstruction.decode (this, instructionWord);

    // execute() returns true if the instruction was completed; false if it
    // was aborted due to an error condition. In that case a trap has been
    // set.
    instr->execute ();
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

void KDF11_A_Cpu::handleTrapsAndInterrupts ()
{
    InterruptRequest intrptReq;
    u16 vectorAddress {0};

    // Traps are handled in order of their priority (cf. EK-KDF11-UG-PR2,
    // par. 10.3.11):
    // 
    // - Memory Management Violation (MMUERR),
    // - Timeout Error (BUSERR),
    // - Parity Error (PARERR) - Unsupported
    // - Trace (T) Bit,
    // - Stack Overflow (STKOVF),
    // - Power Fail (PFAIL),
    // - Interrupt Level 7 (BIRQ7),
    // - Interrupt Level 6 (BIRQ6),
    // - Interrupt Level 5 (BIRQ5),
    // - Interrupt Level 4 (BIRQ4),
    // - Halt Line
    // 
    // Interrupts are only processed if their priority is higher than the
    // current CPU priority. Note that the numerical value of the TrapPriority
    // enum is used as bus request level.
    // 
    // Check if there is a trap or interrupt request to handle. This is the
    // most common case so check this as first.
    if (!trap_ && !bus_->intrptReqAvailable ())
        return;

    // Traps have the highest priority, so first check if there is a trap
    // to handle
    if (trap_ != CpuData::Trap::None)
    {
        handleTrap ();
    }
    else if (bus_->intrptPriority () > cpuPriority ())
        handleInterrupt ();
}

void KDF11_A_Cpu::handleTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (trap_);
    trap_ = CpuData::Trap::None;
}

void KDF11_A_Cpu::handleInterrupt ()
{
    InterruptRequest intrptReq;
 
    if (bus_->getIntrptReq (intrptReq))
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());
}

// Load PC and PSW from the given vector
void KDF11_A_Cpu::loadTrapVector (CpuData::Trap trap)
{
    registers_[7] = fetchWord (trap).valueOr (0);
    psw_ = fetchWord (trap + 2).valueOr (0);
}

u8 KDF11_A_Cpu::cpuPriority()
{
    return (psw_ & PSW_PRIORITY) >> 5;
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
u16 KDF11_A_Cpu::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = fetchWord (address);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

// Swap the PC and PSW with new values from the given vector
void KDF11_A_Cpu::swapPcPSW (u16 vectorAddress)
{
    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!pushWord (psw_) || !pushWord (registers_[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, registers_[6]);
        // ToDo: All interrupts should be cleared?
        trap_ = CpuData::Trap::None;
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
        trap_ = CpuData::Trap::None;
        runState = CpuRunState::HALT;
        haltReason_ = HaltReason::BusErrorOnIntrptVector;
        bus_->SRUN().set (false);
        return;
    }
}

void KDF11_A_Cpu::traceStep ()
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
