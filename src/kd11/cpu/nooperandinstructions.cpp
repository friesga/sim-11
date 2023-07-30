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

// WAIT - wait for interrupt
//
// Condition Codes: not affected
//
// Provides a way for the processor to relinquish use of the bus while it
// waits for an external interrupt request. Having been given a WAIT command,
// the processor will not compete for bus use by fetching instructions or
// operands from memory.
//
// In WAIT, as in all instructions, the PC points to the next instruction
// following the WAIT instruction. Thus when an interrupt causes the PC and PS
// to be pushed onto the processor stack, the address of the next instruction
// following the WAIT is saved. The exit from the interrupt routine (i.e.
// execution of an RTI instruction) will cause resumption of the interrupted
// process at the instruction following the WAIT.
// 
void KD11CPU::WAIT ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, register_[7]);
    runState = CpuState::WAIT;
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

// TRAP - trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (34)
//  PS <- (36)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector 
//
// Operation codes from 104400 to 104777 are TRAP instructions. TRAPs and EMTs
// are identical in operation, except that the trap vector for TRAP is at
// address 34.
//
void KD11CPU::TRAP ()
{
    trace.trap (TrapRecordType::TRAP, 034);
    setTrap (&TRP);
}

// Unused/reserved operation codes
void KD11CPU::unused ()
{
    trace.trap (TrapRecordType::TRAP_ILL, 010);
    setTrap (&illegalInstructionTrap);
}