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
            executeWriteCheck (function);
            break;

        case RKTypes::Seek:
            executeSeek (function.diskAddress);
            break;

        case RKTypes::ReadCheck:
            executeReadCheck (function);
            break;

        case RKTypes::DriveReset:
        case RKTypes::WriteLock:
            break;
            
        default:
            throw logic_error ("Invalid function in RK11D::processFunction");
    }
}


