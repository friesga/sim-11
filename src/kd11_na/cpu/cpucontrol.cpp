#include "kd11cpu.h"
#include "cpucontrol.h"
#include "trace/trace.h"

// Reset the processor
// 
// Clear the registers and the PSW
void KD11CPU::cpuReset ()
{
    // Initialize the registers except for the PC
    for (u16 regNr = 0; regNr <= 6; ++regNr)
        register_[regNr] = 0;

    psw_ = 0;
}

// Reset (the devices on) the bus by setting the INIT signal
void KD11CPU::busReset ()
{
    bus_->BINIT().cycle ();
}

// Halt the processor
void KD11CPU::halt ()
{
    runState = CpuRunState::HALT;
    haltReason_ = HaltReason::HaltInstruction;
    bus_->SRUN().set (false);
    trace.cpuEvent (CpuEventRecordType::CPU_HALT, register_[7]);
}

// Wait for an interrupt
void KD11CPU::wait ()
{
    trace.cpuEvent (CpuEventRecordType::CPU_WAIT, register_[7]);
    runState = CpuRunState::WAIT;
}

// Start the processor at the given address
void KD11CPU::start (u16 address)
{
    register_[7] = address;
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_G, address);
}

// Continue execution at the current PC
void KD11CPU::proceed ()
{
    runState = CpuRunState::RUN;
    bus_->SRUN().set (true);
    trace.cpuEvent (CpuEventRecordType::CPU_ODT_P, register_[7]);
}

// Prevent a trace trap on execution of the next instruction. This is
// used by the RTT instruction.
void KD11CPU::inhibitTraceTrap ()
{
    runState = CpuRunState::INHIBIT_TRACE;
}
