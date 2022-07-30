#include "rl012.h"

// The RL01_2 class is a function object. The operator() function is called
// by the AsyncTimer's handle() function when the timer expires and processes
// the expiration of a timer by calling the controller's completeCommand()
// function.

void RL01_2::operator() ()
{
    owningDevice_->service (*this);
}