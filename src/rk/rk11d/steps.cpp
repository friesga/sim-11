#include "rk11d.h"

// Every RK11 function is executed in a number of steps. Every step is a
// C++ function, returning true if the function is executed succesfully and
// false if not and the RK11 function has to be aborted.
// 
// This file contains the steps for the RK11 functions
//

// Check the in the disk address specified drive is ready
bool RK11D::driveReady (RKTypes::Function function)
{

    if (!rk05Drives_[function.diskAddress.driveSelect]->isReady ())
    {
        setError ([&] {rker_.driveError = 1; });
        return false;
    }

    return true;
}

// Read the given numer of words from the given disk address into the
// controller's buffer.
RKTypes::CommandCompletion RK11D::driveRead (RKTypes::Function function,
    RKTypes::CommandCompletion& commandCompletion)
{
    u16 driveId = function.diskAddress.driveSelect;

    DiskAddress diskAddress
    {
         function.diskAddress.sectorAddress,
         function.diskAddress.surface,
         function.diskAddress.cylinderAddress
    };

    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    return rk05Drives_[driveId]->read (diskAddress,
        wordCount, buffer_.get ());
}

// Read the given number of headers from the given starting disk address
// into the controller's buffer.
RKTypes::CommandCompletion RK11D::driveReadHeader (RKTypes::Function function,
    RKTypes::CommandCompletion& commandCompletion)
{
    u16 driveId = function.diskAddress.driveSelect;

    DiskAddress diskAddress
    {
         function.diskAddress.sectorAddress,
         function.diskAddress.surface,
         function.diskAddress.cylinderAddress
    };

    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    return rk05Drives_[driveId]->readHeader (diskAddress, wordCount,
        buffer_.get ());
}

bool RK11D::driveSeek (RKTypes::Function function,
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