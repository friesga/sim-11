#include "kd11_na_cpucontrol.h"
#include "trace/trace.h"

// Reset the processor
// 
// Clear the registers and the PSW
void KD11_NA_CpuControl::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        cpuData_->registers ()[regNr] = 0;

    cpuData_->psw ().set (PSW::ProtectionMode::Reset, 0);
}

// Reset (the devices on) the bus by setting the INIT signal
void KD11_NA_CpuControl::busReset ()
{
    bus_->BINIT().cycle ();
}

// Halt the processor
void KD11_NA_CpuControl::halt ()
{
    runState = CpuRunState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, cpuData_->registers ()[7]);
}

// Wait for an interrupt
void KD11_NA_CpuControl::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, cpuData_->registers ()[7]);
    runState = CpuRunState::WAIT;
}

// Start the processor at the given address
void KD11_NA_CpuControl::start (u16 address)
{
    cpuData_->registers ()[7] = address;
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
void KD11_NA_CpuControl::proceed ()
{
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpuData_->registers ()[7]);
}
