#include "controllogic.h"

// (Re)boot the system
ControlLogic::State ControlLogic::bootRoutine ()
{
    cpuControl_->cpuReset ();
    bus_->BINIT ().cycle ();
    cpuControl_->start (startAddress_);
    return Running {};
}