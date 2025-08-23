#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;

using RKTypes::rk05Geometry_;

void RK11D::driveWrite (RKTypes::Function const& function,
    RKTypes::FunctionResult& functionResult)
{
    functionResult = rk05Drives_[function.diskAddress.driveSelect]->write (
        function.diskAddress, function.wordCount, buffer_.get ());
}

// The word count in the RKWC register is given as a two's complement
// negative number. To be able to use this value as a word counter 
// it has to be converted to an integer.
u16 RK11D::absValueFromTwosComplement (u16 value) const
{
    return static_cast<u16> (0200000 - value);
}

bool RK11D::readBufferFromMemory (RKTypes::Function const& function,
    RKTypes::FunctionResult& functionResult)
{
    // In the normal case the wordCount words starting at the address in
    // the RKBA are read into the buffer. Setting the RKCS IBA bit inhbits
    // the RKBA from incrementing during the transfer function. This means
    // the buffer will be filled with the pattern from the address in the
    // RKBA.
    if (function.rkcs.inhibitIncrementingRKBA)
        functionResult.statusCode =
            transferPatternToBuffer (function.busAddress,
                function.wordCount, buffer_);
    else
        functionResult.statusCode = transferDataToBuffer (function.busAddress,
            function.wordCount, buffer_);

    return functionResult.statusCode == StatusCode::Success;
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

void RK11D::clearBufferToEndOfSector (RKTypes::Function const& function)
{
    for (u16 index = function.wordCount;
        index < RKTypes::wordCountForEntireSectors (function.wordCount); ++index)
    {
        buffer_[index] = 0;
    }
}


