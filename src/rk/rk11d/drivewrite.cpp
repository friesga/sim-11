#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;

using RKTypes::rk05Geometry_;

void RK11D::driveWrite (RKTypes::Function const& function,
    RKTypes::CommandCompletion& commandCompletion)
{
    commandCompletion = rk05Drives_[function.diskAddress.driveSelect]->write (
        DiskAddress {function.diskAddress.sectorAddress,
        function.diskAddress.surface,
        function.diskAddress.cylinderAddress},
        absValueFromTwosComplement (function.wordCount),
        buffer_.get ());
}

// The word count in the RKWC register is given as a two's complement
// negative number. To be able to use this value as a word counter 
u32 RK11D::absValueFromTwosComplement (u16 value) const
{
    return static_cast<u32> (0200000 - value);
}

bool RK11D::readBufferFromMemory (RKTypes::Function const& function,
    RKTypes::CommandCompletion& commandCompletion)
{
    // In the normal case the wordCount words starting at the address in
    // the RKBA are read into the buffer. Setting the RKCS IBA bit inhbits
    // the RKBA from incrementing during the transfer function. This means
    // the buffer will be filled with the pattern from the address in the
    // RKBA.
    if (function.rkcs.inhibitIncrementingRKBA)
        commandCompletion.statusCode =
            transferPatternToBuffer (function.busAddress,
                function.wordCount, buffer_);
    else
        commandCompletion.statusCode = transferDataToBuffer (function.busAddress,
            function.wordCount, buffer_);

    return commandCompletion.statusCode == StatusCode::Success;
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
