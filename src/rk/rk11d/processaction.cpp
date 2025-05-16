#include "rk11d.h"

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
        case RKTypes::Read:
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
    
    // ToDo: setControlRady() can be moved to processFunction's caller?
    setControlReady ();
}

// The Control Ready bit indicates the controller is ready to perform
// a function. Set by INIT, a hard error condition, or by the termination
// of a function. (EK-RK11D-MM-002 p. 3-6)
void RK11D::setControlReady ()
{
    rkcs_.controlReady = 1;

    if (rkcs_.interruptOnDoneEnable)
        bus_->setInterrupt (TrapPriority::BR5, 5, 0, vector_);
}

// The RK05 uses this function to indicate a changed drive condition to the
// controller.
//
void RK11D::setDriveCondition (RKTypes::DriveCondition condition)
{
    // Guard against controller register access from the RK11D thread
    std::unique_lock<std::mutex> lock {controllerMutex_};

    actionQueue_.push (condition);

    actionAvailable_.notify_one ();
}


// This function is called by the RK05 to pass the result of the execution
// of a command to the controller and is executed in an RK05 thread.
void RK11D::processDriveCondition (RKTypes::DriveCondition driveCondition)
{
    // Guard against controller register access from the processor thread
    std::unique_lock<std::mutex> lock {controllerMutex_};

    rkds_.value = driveCondition.rkds.value;
    rker_.value = driveCondition.rker.value;

    if (rker_.value != 0)
        rkcs_.error = 1;

    if (rker_.hardError != 0)
        rkcs_.hardError = 1;

    // The drive is ready to accept a new command
    // rkds_.driveId = driveCondition.rkds.driveId;
    // rkds_.driveReady = 1;
    setControlReady ();
    
}


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