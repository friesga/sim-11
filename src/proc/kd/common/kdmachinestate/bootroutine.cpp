#include "kdmachinestate.h"

// (Re)boot the system
KDMachineState::State KDMachineState::bootRoutine ()
{
    cpuController_->cpuReset ();
    bus_->BINIT ().cycle ();
    cpuController_->start (startAddress_);
    return Running {};
}