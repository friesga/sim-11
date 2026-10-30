#include "rk11d.h"

// Seek operations are executed either in synchronous or asyncchronous
// mode. In synchronous mode the completion of the seek is waited for,
// in asynchronous mode the seek is simply started and the function
// returns.
bool RK11D::seek (RKTypes::Function const& function, RK11D::SeekMode mode)
{
    if (!cylinderAddressOk (function))
    {
        setSeekError ();
        return false;
    }

    rk05Drives_[function.diskAddress.driveSelect]->seek (function.diskAddress.cylinderAddress);

    if (mode == SeekMode::Sync)
        waitTillSeekCompleted (function.diskAddress.driveSelect);
    
    return true;
}

bool RK11D::cylinderAddressOk (RKTypes::Function const& function)
{
    return (function.diskAddress.cylinderAddress < RKTypes::CylindersPerDisk);
}

void RK11D::setSeekError ()
{
    setError ([&] {rker_.nonexistentCylinder = 1; });
}

// Wait till the sync seek for the given drive is completed. Seek completions
// for other drives are ignored. By definition all received events are
// seek completions as all non-seek functions are synchronous.
void RK11D::waitTillSeekCompleted (u16 driveId)
{
    RKTypes::SeekCompleteReport report {};

    do
    {
        // The Event variant needs an explicicit initialization
        Event event {RKTypes::Function {}};

        functionQueue_.waitAndPop (event);
        report = std::get<RKTypes::SeekCompleteReport> (event);
    }
    while (report.driveId != driveId);
}
