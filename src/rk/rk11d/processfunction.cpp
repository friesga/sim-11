#include "rk11d.h"

// All functions are to be processed by the RK05 drive, except for the
// ControlReset functions.
void RK11D::processFunction (RKDefinitions::RKCommand command)
{
    if (command.function == RKDefinitions::Function::ControlReset)
        reset ();
    else
        // ToDo: Check drive is present
        rk05Drives_[command.diskAddress.driveSelect]->processCommand (command);
}

// This function is called by the RK05 to pass the result of the execution
// of a command to the controller.
void RK11D::processResult (RKDefinitions::Result result)
{
    // Guard against controller register access from the processor thread
    std::unique_lock<std::mutex> lock {controllerMutex_};

    rkds_.value = result.rkds.value;
    rker_.value = result.rker.value;
    rkwc_ = result.wordCount;
    rkba_ = result.busAddress;

    if (rker_.value != 0)
        rkcs_.error = 1;

    if (rker_.hardError != 0)
        rkcs_.hardError = 1;

    rkcs_.controlReady = 1;
}