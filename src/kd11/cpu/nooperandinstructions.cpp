#include "kd11cpu.h"
#include "trace/trace.h"

// HALT - halt
// 
// Condition Codes: not affected
// 
// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
//
void KD11CPU::HALT ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, register_[7]);

    runState = CpuState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN ().set (false);
}

// EMT - emulator trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (30)
//  PS <- (32)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
// All operation codes from 104000 to 104377 are EMT instructions and may be
// used to transmit information to the emulating routine (e.g., function to be
// performed). The trap vector for EMT is at address 30. The new PC is taken
// from the word at address 30; the new processor status (PS) is taken from the
// word at address 32.
//
void KD11CPU::EMT ()
{
    trace.trap (TrapRecordType::TRAP, 030);
    setTrap (&EmulatorTrap);
}

// Unused/reserved operation codes
void KD11CPU::unused ()
{
    trace.trap (TrapRecordType::TRAP_ILL, 010);
    setTrap (&illegalInstructionTrap);
}