#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;

using RKTypes::rk05Geometry_;

// Read the given numer of words from the given disk address into the
// controller's buffer.
void RK11D::driveRead (RKTypes::Function const& function,
    RKTypes::FunctionResult& functionResult)
{
    u16 driveId = function.diskAddress.driveSelect;

    functionResult = rk05Drives_[driveId]->read (function.diskAddress,
        function.wordCount, buffer_.get ());
}

bool RK11D::writeBufferToMemory (RKTypes::Function const& function,
    RKTypes::FunctionResult& functionResult)
{
    // In the normal case the wordCount words starting at the address in
    // the RKBA are written to memory. Setting the RKCS IBA bit inhbits the
    // RKBA from incrementing during the transfer function. This means that
    // just the last word in the buffer will be written to the address in
    // the RKBA.
    if (function.rkcs.inhibitIncrementingRKBA)
        functionResult.statusCode = 
            transferWordFromBuffer (function.busAddress,
                functionResult.wordsTransferred, buffer_);
    else
    {
        functionResult.statusCode = 
            transferDataFromBuffer (function.busAddress,
                functionResult.wordsTransferred, buffer_);
    }

    return functionResult.statusCode == StatusCode::Success;
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