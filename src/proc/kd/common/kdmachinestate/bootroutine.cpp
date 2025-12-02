#include "kdmachinestate.h"

// (Re)boot the system
KDMachineState::State KDMachineState::bootRoutine ()
{
    cpuControl_->cpuReset ();
    bus_->BINIT ().cycle ();
    cpuControl_->start (startAddress_);
    return Running {};
}