#include "kdf11_a_cpu.h"
#include "trace/trace.h"

// Reset the processor
// 
// Clear the registers and the PSW
void KDF11_A_Cpu::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        cpuData_->registers ()[regNr] = 0;

    cpuData_->psw () = 0;
}

// Reset (the devices on) the bus by setting the INIT signal and reset
// the KTF11-A.
void KDF11_A_Cpu::busReset ()
{
    bus_->BINIT().cycle ();
    mmu_->reset ();
}

// Halt the processor
void KDF11_A_Cpu::halt ()
{
    runState = CpuRunState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, cpuData_->registers ()[7]);
}

// Wait for an interrupt
void KDF11_A_Cpu::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, cpuData_->registers ()[7]);
    runState = CpuRunState::WAIT;
}

// Start the processor at the given address
void KDF11_A_Cpu::start (u16 address)
{
    cpuData_->registers ()[7] = address;
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
void KDF11_A_Cpu::proceed ()
{
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, cpuData_->registers ()[7]);
}

