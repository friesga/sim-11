#include "rk11d.h"
#include "trace/trace.h"

using std::function;

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
    {
        trace.debug ("setControlReady - Request interrupt");
        bus_->requestInterrupt (TrapPriority::BR5, 5, 0, vector_);
    }

    rkcs_.controlReady = 1;
}

bool RK11D::functionParametersOk (RKTypes::Function function)
{
    // Check validity of the function's parameters
    if (function.diskAddress.sectorAddress >= RKTypes::SectorsPerSurface)
    {
        setError ([&] {rker_.nonexistentSector = 1; });
        return false;
    }

    if (function.diskAddress.cylinderAddress >= RKTypes::CylindersPerDisk)
    {
        setError ([&] {rker_.nonexistentCylinder = 1; });
        return false;
    }

    return true;
}

// The drive status for a non-existing disk is returned when the RKDS
// is read for the non-existing disk.
void RK11D::setNonExistingDisk ()
{
    rker_.driveError = 1;

    setError ([&] {rker_.nonExistentDisk = 1;});
}

// RKCS HE sets when any of RKER 05—15 are set. RKCS ERR sets when any bit
// of the RKER sets.
void RK11D::setError (function<void ()> function)
{
    function ();

    rkcs_.error = 1;

    if (rker_.hardError != 0)
        rkcs_.hardError = 1;
}

RKTypes::RKDS RK11D::getDriveStatus (u16 driveId)
{
    if (selectedDrive_ < rk05Drives_.size ())
        return rk05Drives_[selectedDrive_]->driveStatus ();
    else
    {
        RKTypes::RKDS rkds {};
        rkds.driveId = selectedDrive_;
        rkds.drivePowerLow = 1;
        rkds.driveUnsafe = 1;
        rkds.driveReady = 0;
        return rkds;
    }
}