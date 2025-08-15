#include "rk11d.h"

#include <variant>

using std::get;

// The function is already safeguarded against register access by the
// CPU thread as the controllerMutex_ is locked by the action processor.
//
void RK11D::processFunction (RKTypes::Function function)
{
    switch (function.rkcs.operation)
    {
        case RKTypes::ControlReset:
            reset ();
            break;

        case RKTypes::Write:
            executeWrite (function);
            break;

        case RKTypes::Read:
            if (function.rkcs.format)
                executeReadHeader (function);
            else
                executeRead (function);
            break;

        case RKTypes::WriteCheck:
            executeWriteCheck (function);
            break;

        case RKTypes::Seek:
            executeSeek (function);
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

    setControlReady ();
}


