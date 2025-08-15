#include "rk11d.h"

#include <algorithm>

using std::ranges::all_of;

// For a Seek function, the RK11 directs the selected disk drive to move its
// head mechanism to the cylinder address specified by RKDA 05 through 12.
// When this portion of a Seek has been initiated, the controller returns to
// the Ready state (RKCS 07). But if the specified cylinder address is greater
// than 0312, the function is aborted and bit 06 (nonexistent Cylinder) of the
// RKER is set. RKCS 06 (Interrupt Done Enable) then determines the program
// reaction. (EK-RK11D-MM-002 par. 1.3.2.2)
//
// The acceptance (Address Acknowledge) of a Seek or Srive Reset function by
// the selected drive generates an interrupt request.
// (EK-RK11D-MM-002, par. 3.4)
// 
// Presumably the interrupt is only generated when RKCS IDE is set.
//
void RK11D::executeSeek (RKTypes::Function function)
{
    RKTypes::CommandCompletion commandCompletion {};

    all_of (seekFunction_, [&] (auto& f)
        { return f (function, commandCompletion); });
}

bool RK11D::cylinderAddressOk (RKTypes::Function function)
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

// Execute an asynchronous seek, i.e. start a seek not waiting for its
// completion.
bool RK11D::asyncSeek (RKTypes::Function function)
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
bool RK11D::syncSeek (RKTypes::Function function,
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
