#include "kd11_na_odt.h"

// This file contains the entry actions and state transitions for
// the state ExitPoint. Entering this state signifies that ODT has to exit. 
void KD11_NA_ODT::StateMachine::entry (ExitPoint)
{
    // Set the CPU in the running state and exit the ODT state machine
    context_->odtRunning_ = false;
}