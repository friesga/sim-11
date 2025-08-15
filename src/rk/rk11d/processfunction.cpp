#include "rk11d.h"

#include <algorithm>

using std::ranges::all_of;

// The function is already safeguarded against register access by the
// CPU thread as the controllerMutex_ is locked by the action processor.
//
void RK11D::processFunction (RKTypes::Function function)
{
    RKTypes::CommandCompletion commandCompletion {};
    StepVector sequenceToBeExecuted {};

    if (function.rkcs.operation == RKTypes::Read && function.rkcs.format)
        sequenceToBeExecuted = readHeaderFunction_;
    else
        sequenceToBeExecuted = rk11dFunctions[function.rkcs.operation];

    all_of (sequenceToBeExecuted, [&] (auto& f)
        { return f (function, commandCompletion); });

    setControlReady ();
}


