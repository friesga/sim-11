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

// The Control Ready bit indicates the controller is ready to perform
// a function. Set by INIT, a hard error condition, or by the termination
// of a function.
// 
// When Interrupt on Done is enabled the controller issues an interrupt
// request if:
// - A function has cvompleted activity,
// - A hard error is encountered,
// - A soft error is encountered and bit 08 of the RKCS (SSE) is set,
// RKCS 07 (RDY) is set and GO is not set.
// 
// (EK-RK11D-MM-002 p. 3-5/6)
//
void RK11D::setControlReady ()
{
    // The interrupt request is generated before the controller is set ready
    // to facilitate the unit tests waiting for the interrupt. This might not
    // be actually correct behaviour.
    if (rkcs_.interruptOnDoneEnable)
        bus_->setInterrupt (TrapPriority::BR5, 5, 0, vector_);

    rkcs_.controlReady = 1;
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