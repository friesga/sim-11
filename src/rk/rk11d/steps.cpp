#include "rk11d.h"

#include <stdexcept>

using std::out_of_range;

using RKTypes::rk05Geometry_;

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

bool RK11D::notWriteProtected (RKTypes::Function function)
{
    if (rk05Drives_[function.diskAddress.driveSelect]->isWriteProtected ())
    {
        setError ([&] {rker_.writeLockoutViolation = 1; });
        return false;
    }

    return true;
}

// Read the given number of headers from the given starting disk address
// into the controller's buffer.
void RK11D::driveReadHeader (RKTypes::Function function,
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

    commandCompletion = rk05Drives_[driveId]->readHeader (diskAddress,
        wordCount, buffer_.get ());
}

bool RK11D::updateRegisters (RKTypes::Function function,
    RKTypes::CommandCompletion& commandCompletion)
{
    if (!function.rkcs.inhibitIncrementingRKBA)
    {
        busAddressToRegs (function.busAddress +
            commandCompletion.wordsTransferred * 2);
    }

    // The bits of [the RKDB] register work as a general data handler in that
    // all information transferred between the control[ler] and the disk drive
    // must pass through this register. (EK-RK11D-MM-002, p. 3-8). 
    // 
    // After 1 sector read RKDB contains for RK11C the checksum for that sector,
    // for RK11D the last word transferred to memory. (CZRKKF0, line 3074)
    //
    rkwc_ += commandCompletion.wordsTransferred;
    rkdb_ = buffer_[commandCompletion.wordsTransferred - 1];

    // An increment of the RKDA might overflow the logical block number.
    // 
    // RKER OVR indicates that, during a Read, Write, Read Check, or Write
    // Check function, operations on sector 013, surface 1 of cylinder address
    // 0312 were finished, and the RKWC has not yet overflowed.This is
    // essentially an attempt to overflow out of a disk drive.
    // (EK-RK11D-MM-002, p. 3-4)
    try
    {
        rkda_ += commandCompletion.sectorsProcessed;
    }
    catch (out_of_range)
    {
        rkda_ = rk05Geometry_.lbnTodiskAddress (rk05Geometry_.diskCapacity () - 1);

        if (commandCompletion.wordsTransferred <
            absValueFromTwosComplement (function.wordCount))
        {
            setError ([&] {rker_.overrun = 1; });
            return false;
        }
    }

    return true;
}

bool RK11D::resultOk (RKTypes::Function function,
    RKTypes::CommandCompletion& commandCompletion)
{
    if (commandCompletion.wordsTransferred <
        absValueFromTwosComplement (function.wordCount))
    {
        setError ([&] {rker_.overrun = 1; });
        return false;
    }

    return true;
}
