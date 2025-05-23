#include "rk11d.h"

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

void RK11D::setDriveError ()
{
    rker_.driveError = 1;
    rkcs_.error = 1;
    rkcs_.hardError = 1;
}