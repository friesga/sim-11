#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;
using RKTypes::rk05Geometry_;

// ToDo: Pass Function as argument?
void RK11D::executeWrite (RKTypes::Function function)
{
    CommandCompletion commandCompletion {};
    u16 driveId = function.diskAddress.driveSelect;

    // Check the drive is ready
    if (!rk05Drives_[driveId]->isReady ())
    {
        setError ([&] {rker_.driveError = 1; });
        return;
    }

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;

    // In the normal case the wordCount words starting at the address in
    // the RKBA are read into the buffer. Setting the RKCS IBA bit inhbits
    // the RKBA from incrementing during the transfer function. This means
    // the buffer will be filled with the pattern from the address in the
    // RKBA.
    StatusCode status {};
    if (function.rkcs.inhibitIncrementingRKBA)
        status = transferPatternToBuffer (function.busAddress,
            function.wordCount, buffer_);
    else
        status = transferDataToBuffer (function.busAddress,
            function.wordCount, buffer_);

    if (status != StatusCode::Success)
    {
        // Set error condition
        return;
    }

    // Clear to end of block

    // Stop possible running hardware poll
    pollEventQueue_.push (StopPoll {});

    // Command RK05 to write data from buffer to disk
    rk05Drives_[driveId]->write (
        DiskAddress {function.diskAddress.sectorAddress,
        function.diskAddress.surface,
        function.diskAddress.cylinderAddress},
        absValueFromTwosComplement (function.wordCount),
        buffer_.get ());

    // Await the result of the execution of the write
    commandCompletionQueue_.waitAndPop (commandCompletion);

    // Adjust RKWC and - in case IBA isn't set - the RKBA
    rkwc_ += commandCompletion.wordsTransferred;

    if (!function.rkcs.inhibitIncrementingRKBA)
        rkba_ += (commandCompletion.wordsTransferred * 2);

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

// The word count in the RKWC register is given as a two's complement
// negative number. To be able to use this value as a word counter 
u32 RK11D::absValueFromTwosComplement (u16 value) const
{
    return static_cast<u32> (0200000 - value);
}

StatusCode RK11D::transferDataToBuffer (BusAddress memoryAddress,
    u16 wordCount, unique_ptr<u16[]>& buffer)
{
    for (size_t index = 0; index < wordCount; memoryAddress += 2, ++index)
    {
        CondData<u16> value = bus_->read (memoryAddress).valueOr (0);
        if (!value.hasValue ())
            return StatusCode::NonExistingMemory;

        buffer_[index] = value;
    }

    return StatusCode::Success;
}

StatusCode RK11D::transferPatternToBuffer (BusAddress memoryAddress,
    u16 wordCount, unique_ptr<u16[]>& buffer)
{
    CondData<u16> pattern = bus_->read (memoryAddress).valueOr (0);
    if (!pattern.hasValue ())
        return StatusCode::NonExistingMemory;

    for (size_t index = 0; index < wordCount; ++index)
        buffer_[index] = pattern;

    return StatusCode::Success;
}

void RK11D::dataTransferComplete (StatusCode statusCode, u16 wordsTransferred,
    u16 sectorsProcessed)
{
    commandCompletionQueue_.push (CommandCompletion {statusCode, wordsTransferred,
        sectorsProcessed});
}