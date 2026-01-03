#include "basicprocessorexceptionhandler.h"

BasicProcessorExceptionHandler::BasicProcessorExceptionHandler (Bus* bus, CpuData* cpuData,
    Interfaces::CpuController* cpuController, MMU* mmu)
    :
    bus_ {bus},
    cpuData_ (cpuData),
    cpuController_ {cpuController},
    mmu_ {mmu}
{}

void BasicProcessorExceptionHandler::serviceTrap ()
{
    // The enum trap_ is converted to the u16 vector address
    // Swap the PC and PSW with new values from the trap vector to process.
    // If this fails the processor will be put in the HALT state.
    swapPcPSW (cpuData_->trapVector ());
    cpuData_->clearTrap ();
}

void BasicProcessorExceptionHandler::serviceInterrupt ()
{
    InterruptRequest intrptReq;

    if (bus_->getIntrptReq (intrptReq))
        // Swap the PC and PSW with new values from the trap vector to process.
        // If this fails the processor will be put in the HALT state.
        swapPcPSW (intrptReq.vector ());
}

// Swap the PC and PSW with new values from the given vector
void BasicProcessorExceptionHandler::swapPcPSW (u16 vectorAddress)
{
    trace.cpuEvent (CpuEventRecordType::CPU_TRAP, vectorAddress);

    // Save PC and PSW on the stack. Adressing the stack could result in a
    // bus time out. In that case the CPU is halted.
    if (!mmu_->pushWord (cpuData_->psw ()) || !mmu_->pushWord (cpuData_->registers ()[7]))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, cpuData_->registers ()[6]);
        // ToDo: All interrupts should be cleared?
        cpuData_->clearTrap ();
        cpuController_->halt (Interfaces::CpuController::HaltReason::DoubleBusError);
        return;
    }

    // Read new PC and PSW from the trap vector. These read's could also
    // result in a bus time out.
    if (!fetchFromVector (vectorAddress, &cpuData_->registers ()[7]) ||
        !fetchFromVector (vectorAddress + 2, [this] (u16 value)
            {cpuData_->psw ().set (PSW::ProtectionMode::Trap, value); }))
    {
        trace.cpuEvent (CpuEventRecordType::CPU_DBLBUS, vectorAddress);
        cpuData_->clearTrap ();
        cpuController_->halt (Interfaces::CpuController::HaltReason::BusErrorOnIntrptVector);
        return;
    }
}

// Fetch PC and PSW from the given vector address. If this fails the
// processor will halt anyway.
bool BasicProcessorExceptionHandler::fetchFromVector (u16 address, u16* dest)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address, PSW::Mode::Kernel);
    *dest = tmpValue.valueOr (0);
    return tmpValue.hasValue ();
}

bool BasicProcessorExceptionHandler::fetchFromVector (u16 address, function<void (u16)> lambda)
{
    CondData<u16> tmpValue = mmu_->fetchWord (address);
    lambda (tmpValue.valueOr (0));
    return tmpValue.hasValue ();
}