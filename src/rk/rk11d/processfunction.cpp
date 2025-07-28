#include "rk11d.h"

// The function is already safeguarded against register access by the
// CPU thread as the controllerMutex_ is locked by the action processor.
//
void RK11D::processFunction (RKTypes::Function function)
{
    switch (function.rkcs.operation)
    {
        case RKTypes::ControlReset:
            reset ();
            setControlReady ();
            break;

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
            executeDriveReset (function);
            break;

        case RKTypes::WriteLock:
            executeWriteLock (function);
            break;
            
        default:
            throw logic_error ("Invalid function in RK11D::processFunction");
    }
}


