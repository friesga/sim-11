#include "kd11odt.h"

using namespace KD11_F;

// This file contains the entry actions and state transitions for
// the state ExitPoint. Entering this state signifies that ODT has to exit. 
void KD11ODT::entry (ExitPoint)
{
    // Indicate an exit of the state machine
    odtRunning_ = false;
}