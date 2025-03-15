#include "kdf11_cpucontrol.h"
#include "trace/trace.h"
#include "chrono/simulatorclock/simulatorclock.h"

using namespace std::chrono;

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

// This function is called on the execution of a WAIT instruction. It waits
// for a signal or interrupt request finishing the WAIT instruction.
// If no signal or an interrupt request is present advance time so devices
// are awakened at the specified time.
void KDF11_CpuControl::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, cpuData_->registers ()[7]);
    
    while (!signalSet () && !bus_->intrptReqAvailable ())
        SimulatorClock::forwardClock (microseconds (50));
    
    trace.cpuEvent (CpuEventRecordType::CPU_RUN, cpuData_->registers ()[7]);
    bus_->SRUN ().set (true);
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

// The function return true if any for the wait state relevant signals changes
// to a relevant state.
bool KDF11_CpuControl::signalSet ()
{
    if (!bus_->BPOK () ||
         bus_->RESET () ||
         bus_->BHALT () ||
         bus_->BINIT () ||
         bus_->BOOT ())
            return true;

    return false;
}

