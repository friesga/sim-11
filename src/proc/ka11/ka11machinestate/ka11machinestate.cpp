#include "ka11machinestate.h"

KA11MachineState::KA11MachineState (Bus* bus, CpuData* cpuData,
    Interfaces::CpuController* cpuController, MMU* mmu, KY11Console& ky11Console)
    :
    BaseMachineState (bus, cpuData, cpuController, mmu),
    ky11Console_ {ky11Console}
{}

BaseMachineState::State KA11MachineState::powerUpRoutine ()
{
    if (ky11Console_.haltEnablePosition () == KY11Console::HaltEnablePosition::Enable)
        return Running {};
    else
        return Halted {};
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