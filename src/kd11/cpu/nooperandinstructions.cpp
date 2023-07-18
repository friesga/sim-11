#include "kd11cpu.h"
#include "trace/trace.h"

void KD11CPU::HALT ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, register_[7]);

    runState = CpuState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN ().set (false);
}