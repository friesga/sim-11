#include "rk11d.h"

// All functions are to be processed by the RK05 drive, except for the
// ControlReset functions.
void RK11D::processFunction (u16 function)
{
    if (function == ControlReset)
        reset ();
}