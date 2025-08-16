#include "rk11d.h"

// This function executes the RK11 function by execution all steps
// in the sequence for that function.
// 
// The function is already safeguarded against register access by the
// CPU thread as the controllerMutex_ is locked by the function processor.
//
void RK11D::processFunction (RKTypes::Function const& function)
{
    RKTypes::CommandCompletion commandCompletion {};
    StepVector sequenceToBeExecuted {};

    // The Read Header function is a special case in the sense that it
    // doesn't have its own operation code, but is selected by the
    // combination of the Read operation code and setting of the format
    // bit.
    if (function.rkcs.operation == RKTypes::Read && function.rkcs.format)
        sequenceToBeExecuted = readHeaderFunction_;
    else
        sequenceToBeExecuted = rk11dFunctions[function.rkcs.operation];

    for (auto step : sequenceToBeExecuted)
    {
        if (!(step)(function, commandCompletion))
            break;
    }

    setControlReady ();
}


