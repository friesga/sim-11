#include "rk11d.h"

// Every RK11 function is executed in a number of steps. Every step is a
// C++ function, returning true if the function is executed succesfully and
// false if not and the RK11 function has to be aborted.
// 
// This file contains the steps for the RK11 functions
//

// Check the in the disk address specified drive is ready
bool RK11D::driveReady (RKTypes::Function function)
{

    if (!rk05Drives_[function.diskAddress.driveSelect]->isReady ())
    {
        setError ([&] {rker_.driveError = 1; });
        return false;
    }

    return true;
}