#include "rk11d.h"

void RK11D::processAction (RKDefinitions::Function action)
{
    u16 driveId = action.diskAddress.driveSelect;

    if (driveId >= rk05Drives_.size ())
    {
        setNonExistingDisk (driveId);
        return;
    }

    switch (action.operation)
    {
        case RKDefinitions::ControlReset:
            reset ();
            break;

        case RKDefinitions::Write:
        case RKDefinitions::Read:
        case RKDefinitions::WriteCheck:
        case RKDefinitions::Seek:
        case RKDefinitions::ReadCheck:
        case RKDefinitions::DriveReset:
        case RKDefinitions::WriteLock:

        default:
            throw logic_error ("Invalid function in RK11D::processAction");
    }

}

#if 0       // To be deleted


// This function is called by the RK05 to pass the result of the execution
// of a command to the controller and is executed in an RK05 thread.
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

    // The drive is ready to accept a new command
    rkds_.driveId = result.rkds.driveId;
    rkds_.driveReady = 1;
}
#endif

void RK11D::setNonExistingDisk (u16 driveId)
{
    rkds_.driveId = driveId;
    rkds_.drivePowerLow = 1;
    rkds_.driveUnsafe = 1;
    rkds_.driveReady = 0;
    rker_.driveError = 1;
    rker_.nonExistentDisk = 1;
    rkcs_.error = 1;
    rkcs_.hardError = 1;
}