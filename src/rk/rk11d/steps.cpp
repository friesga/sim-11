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
void RK11D::driveRead (RKTypes::Function function,
    CommandCompletion& commandCompletion)
{
    u16 driveId = function.diskAddress.driveSelect;

    DiskAddress diskAddress
    {
         function.diskAddress.sectorAddress,
         function.diskAddress.surface,
         function.diskAddress.cylinderAddress
    };

    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    rk05Drives_[driveId]->read (diskAddress,
        wordCount, buffer_.get ());

    // Await the result of the execution of the read
    commandCompletionQueue_.waitAndPop (commandCompletion);
}

// Read the given number of headers from the given starting disk address
// into the controller's buffer.
void RK11D::driveReadHeader (RKTypes::Function function,
    CommandCompletion& commandCompletion)
{
    u16 driveId = function.diskAddress.driveSelect;

    DiskAddress diskAddress
    {
         function.diskAddress.sectorAddress,
         function.diskAddress.surface,
         function.diskAddress.cylinderAddress
    };

    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    rk05Drives_[driveId]->readHeader (diskAddress, wordCount,
        buffer_.get ());

    // Await the result of the execution of the read
    commandCompletionQueue_.waitAndPop (commandCompletion);
}