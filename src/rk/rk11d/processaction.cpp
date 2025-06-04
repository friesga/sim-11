#include "rk11d.h"

// The function is already safeguarded against register access by the
// CPU thread as the controllerMutex_ is locked by the action processor.
//
void RK11D::processFunction (RKTypes::Function function)
{
    // A Control Reset can be performed without any RK05 drive attached
    if (function.operation == RKTypes::ControlReset)
    {
        reset ();
        setControlReady ();
        return;
    }

    u16 driveId = function.diskAddress.driveSelect;

    if (driveId >= rk05Drives_.size ())
    {
        setNonExistingDisk (driveId);
        setControlReady ();
        return;
    }

    switch (function.operation)
    {
        case RKTypes::Write:
            executeWrite (function);
            break;

        case RKTypes::Read:
            executeRead (function);
            break;

        case RKTypes::WriteCheck:
            break;

        case RKTypes::Seek:
            executeSeek (function.diskAddress);
            break;

        case RKTypes::ReadCheck:
        case RKTypes::DriveReset:
        case RKTypes::WriteLock:
            break;
            
        default:
            throw logic_error ("Invalid function in RK11D::processFunction");
    }
}


// The RK05 uses this function to indicate a changed drive condition to the
// controller.
//
void RK11D::setDriveCondition (RKTypes::DriveCondition condition)
{
    // Guard against controller register access from the RK11D thread
    std::lock_guard<std::mutex> guard {controllerMutex_};

    actionQueue_.push (condition);

    actionAvailable_.notify_one ();
}


// This function is called by the RK05 to pass the result of the execution
// of a command to the controller and is executed in an RK05 thread.
// 
// The function is already safeguarded against register access by the
// CPU thread as the controllerMutex_ is locked by the action processor.
//
void RK11D::processDriveCondition (RKTypes::DriveCondition driveCondition)
{
    rkds_.value = driveCondition.rkds.value;
    rker_.value = driveCondition.rker.value;

    if (rker_.value != 0)
        rkcs_.error = 1;

    if (rker_.hardError != 0)
        rkcs_.hardError = 1;

    setControlReady ();
}
