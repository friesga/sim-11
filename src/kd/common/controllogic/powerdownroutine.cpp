#include "controllogic.h"

// This function is called in the transitions from PowerFail when either
// the DC voltage is low or the systems halts. The new state will be either
// PowerOff or Standby, depending on the state of the battery power (if
// present).
ControlLogic::State ControlLogic::powerDownRoutine ()
{
    bus_->SRUN ().set (false);

    // A condensed if-statement isn't allowed as StandBy and PowerOff
    // are incompatible operand types.
    if (bus_->BatteryPower ())
        return Standby {};
    else 
        return PowerOff {};
}