#include "rk11d.h"

// Execute an asynchronous seek, i.e. start a seek not waiting for its
// completion.
bool RK11D::asyncSeek (RKTypes::Function const& function)
{
    if (cylinderAddressOk (function))
    {
        rk05Drives_[function.diskAddress.driveSelect]->seek (function.diskAddress.cylinderAddress);
        return true;
    }
    else
        return false;
}

// Execute a synchronous seek, i.e. start a seek and wait for its completion.
bool RK11D::syncSeek (RKTypes::Function const& function,
    RKTypes::CommandCompletion& commandCompletion)
{
    if (function.diskAddress.cylinderAddress < RKTypes::CylindersPerDisk)
    {
        rk05Drives_[function.diskAddress.driveSelect]->seek (function.diskAddress.cylinderAddress);
        waitTillSeekCompleted (function.diskAddress.driveSelect);
        return true;
    }
    else
    {
        rker_.nonexistentCylinder = 1;
        rkcs_.error = 1;
        rkcs_.hardError = 1;
        return false;
    }
}

bool RK11D::cylinderAddressOk (RKTypes::Function const& function)
{
    if (function.diskAddress.cylinderAddress < RKTypes::CylindersPerDisk)
        return true;
    else
    {
        rker_.nonexistentCylinder = 1;
        rkcs_.error = 1;
        rkcs_.hardError = 1;
        return false;
    }
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
    } while (report.driveId != driveId);
}
