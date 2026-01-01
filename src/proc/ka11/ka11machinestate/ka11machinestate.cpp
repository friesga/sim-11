#include "ka11machinestate.h"

KA11MachineState::KA11MachineState (Bus* bus, CpuData* cpuData,
    Interfaces::CpuController* cpuControl, MMU* mmu)
    :
    BaseMachineState (bus, cpuData, cpuControl, mmu)
{}

BaseMachineState::State KA11MachineState::powerUpRoutine ()
{
    return Running {};
}

BaseMachineState::State KA11MachineState::powerDownRoutine ()
{
    return PowerOff {};
}

BaseMachineState::State KA11MachineState::bootRoutine ()
{
    return Running {};
}

void KA11MachineState::runODT ()
{
}