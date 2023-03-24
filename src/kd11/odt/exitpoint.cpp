#include "kd11odt.h"

using namespace KD11_F;

// This file contains the entry actions and state transitions for
// the state ExitPoint. Entering this state signifies that ODT has to exit. 
void KD11ODT::entry (ExitPoint)
{
    // Set the CPU in the running state and exit the ODT state machine
    cpu_.runState = STATE_RUN;
    bus_->setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::True);
    odtRunning_ = false;
}