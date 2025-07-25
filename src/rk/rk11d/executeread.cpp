#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;
using RKTypes::rk05Geometry_;

void RK11D::executeRead (RKTypes::Function function)
{ 
    CommandCompletion commandCompletion {};
    u16 driveId = function.diskAddress.driveSelect;

    // Check the drive is ready
    if (!rk05Drives_[driveId]->isReady ())
    {
        setError ([&] {rker_.driveError = 1;});
        return;
    }

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;

    // ToDo: Check for sector overflow

    // Stop possible running hardware poll
    pollEventQueue_.push (StopPoll {});

    // Get the parameters ready for the read() or readHeader() call
    // to avoid code duplication
    DiskAddress diskAddress
    {
         function.diskAddress.sectorAddress,
         function.diskAddress.surface,
         function.diskAddress.cylinderAddress
    };
    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    // Either read the header of the given sector(s) in Format Mode or
    // the sector(s) itself/themselves.
    if (function.rkcs.format)
        rk05Drives_[driveId]->readHeader (diskAddress, wordCount,
            buffer_.get ());
    else
        rk05Drives_[driveId]->read (diskAddress, wordCount, buffer_.get ());

    // Await the result of the execution of the read
    commandCompletionQueue_.waitAndPop (commandCompletion);

    // ToDo: Clear the part of the buffer not filled by the read

    // In the normal case the wordCount words starting at the address in
    // the RKBA are written to memory. Setting the RKCS IBA bit inhbits the
    // RKBA from incrementing during the transfer function. This means that
    // just the last word in the buffer will be written to the address in
    // the RKBA.
    if (function.rkcs.inhibitIncrementingRKBA)
        transferWordFromBuffer (function.busAddress,
            commandCompletion.wordsTransferred, buffer_);
    else
    {
        transferDataFromBuffer (function.busAddress,
            commandCompletion.wordsTransferred, buffer_);
        rkba_ += (commandCompletion.wordsTransferred * 2);
    }

    rkwc_ += commandCompletion.wordsTransferred;

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
            setError ([&] {rker_.overrun = 1; });
    }
}

StatusCode RK11D::transferDataFromBuffer (BusAddress memoryAddress,
    u16 wordCount, unique_ptr<u16[]>& buffer)
{
    for (size_t index = 0; index < wordCount; memoryAddress += 2, ++index)
    {
        if (!bus_->writeWord (memoryAddress, buffer_[index]))
            return StatusCode::NonExistingMemory;
    }

    return StatusCode::Success;
}

// Transfer the data in the buffer to memory without incrementing the
// bus address. This corresponds to transferring the last word of the buffer
// to the given bus address.
StatusCode RK11D::transferWordFromBuffer (BusAddress memoryAddress,
    u16 wordCount, unique_ptr<u16[]>& buffer)
{
    if (!bus_->writeWord (memoryAddress, buffer_[wordCount - 1]))
        return StatusCode::NonExistingMemory;

    return StatusCode::Success;
}