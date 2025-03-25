#include "kdf11_cpucontrol.h"
#include "trace/trace.h"

// Constructor
KDF11_CpuControl::KDF11_CpuControl (Bus* bus, CpuData* cpuData, MMU* mmu)
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

// Reset the processor
// 
// Clear the registers and the PSW
void KDF11_CpuControl::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        cpuData_->registers ()[regNr] = 0;

    cpuData_->psw ().set (PSW::ProtectionMode::Reset, 0);
}

// Reset (the devices on) the bus by setting the INIT signal and reset
// the KTF11-A.
void KDF11_CpuControl::busReset ()
{
    bus_->BINIT().cycle ();
    mmu_->reset ();
}

// Halt the processor
void KDF11_CpuControl::halt ()
{
    runState = CpuRunState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, cpuData_->registers ()[7]);
}

void KDF11_CpuControl::wait ()
{
    runState = CpuRunState::WAIT;
}

// Start the processor at the given address
void KDF11_CpuControl::start (u16 address)
{
    cpuData_->registers ()[7] = address;
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
void KDF11_CpuControl::proceed ()
{
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpuData_->registers ()[7]);
}


