#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;

using RKTypes::rk05Geometry_;


bool RK11D::compareBufferWithMemory (RKTypes::Function const& function,
    RKTypes::CommandCompletion& commandCompletion)
{
    if (function.rkcs.inhibitIncrementingRKBA)
    {
        commandCompletion.statusCode =
            comparePatternWithBuffer (function.busAddress,
                commandCompletion.wordsTransferred, buffer_);
    }
    else
    {
        commandCompletion.statusCode = 
            compareDataWithBuffer (function.busAddress,
                commandCompletion.wordsTransferred, buffer_);
    }

    return commandCompletion.statusCode == StatusCode::Success;
}

// Compare the memory contents with the data in the buffer
StatusCode RK11D::compareDataWithBuffer (BusAddress memoryAddress,
    u16 wordCount, unique_ptr<u16[]>& buffer)
{
    for (size_t index = 0; index < wordCount; memoryAddress += 2, ++index)
    {
        CondData<u16> value = bus_->read (memoryAddress).valueOr (0);
        if (!value.hasValue ())
            return StatusCode::NonExistingMemory;

        if (value != buffer_[index])
            return StatusCode::ParityError;
    }

    return StatusCode::Success;
}

// Compare the data in the buffer to memory without incrementing the
// bus address. As a result the data in the buffer is compared with the
// pattern at the given memory address.
StatusCode RK11D::comparePatternWithBuffer (BusAddress memoryAddress,
    u16 wordCount, unique_ptr<u16[]>& buffer)
{
    CondData<u16> value = bus_->read (memoryAddress).valueOr (0);
    if (!value.hasValue ())
        return StatusCode::NonExistingMemory;

    for (size_t index = 0; index < wordCount; ++index)
    {
        if (value != buffer_[index])
            return StatusCode::ParityError;
    }

    return StatusCode::Success;
}

void RK11D::setWritCheckOnError (RKTypes::CommandCompletion& commandCompletion)
{
    if (commandCompletion.statusCode != StatusCode::Success)
        rker_.writeCheckError = 1;
}