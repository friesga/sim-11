#include "kd11cpu.h"
#include "trace/trace.h"

// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
void KD11CPU::HALT ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, register_[7]);

    runState = CpuState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN ().set (false);
}

// Unused/reserved operation codes
void KD11CPU::unused ()
{
    trace.trap (TrapRecordType::TRAP_ILL, 010);
    setTrap (&illegalInstructionTrap);
}