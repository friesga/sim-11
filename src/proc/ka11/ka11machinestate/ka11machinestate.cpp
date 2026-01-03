#include "ka11machinestate.h"

KA11MachineState::KA11MachineState (Bus* bus, CpuData* cpuData,
    Interfaces::CpuController* cpuController, MMU* mmu)
    :
    BaseMachineState (bus, cpuData, cpuController, mmu)
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