#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;
using RKTypes::rk05Geometry_;

// ToDo: Pass Function as argument?
void RK11D::executeWrite (RKTypes::Function function)
{
    RKTypes::CommandCompletion commandCompletion {};
    u16 driveId = function.diskAddress.driveSelect;

    if (!driveReady (function))
        return;

    // Check the drive is not write-protected
    if (rk05Drives_[driveId]->isWriteProtected ())
    {
        setError ([&] {rker_.writeLockoutViolation = 1; });
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

    if (!driveSeek (function, commandCompletion))
        return;

    // Command RK05 to write data from buffer to disk
    commandCompletion = rk05Drives_[driveId]->write (
        DiskAddress {function.diskAddress.sectorAddress,
        function.diskAddress.surface,
        function.diskAddress.cylinderAddress},
        absValueFromTwosComplement (function.wordCount),
        buffer_.get ());


    updateRegisters (function, commandCompletion);
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
