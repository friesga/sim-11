#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>
#include <algorithm>

using std::out_of_range;
using std::ranges::all_of;

using RKTypes::rk05Geometry_;

void RK11D::executeRead (RKTypes::Function function)
{ 
    RKTypes::CommandCompletion commandCompletion {};

    all_of (readFunction_, [&] (auto& f)
        { return f (function, commandCompletion); });
}

// Read the given numer of words from the given disk address into the
// controller's buffer.
void RK11D::driveRead (RKTypes::Function function,
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

    commandCompletion = rk05Drives_[driveId]->read (diskAddress,
        wordCount, buffer_.get ());
}

bool RK11D::writeBufferToMemory (RKTypes::Function function,
    RKTypes::CommandCompletion& commandCompletion)
{
    // In the normal case the wordCount words starting at the address in
    // the RKBA are written to memory. Setting the RKCS IBA bit inhbits the
    // RKBA from incrementing during the transfer function. This means that
    // just the last word in the buffer will be written to the address in
    // the RKBA.
    if (function.rkcs.inhibitIncrementingRKBA)
        commandCompletion.statusCode = 
            transferWordFromBuffer (function.busAddress,
                commandCompletion.wordsTransferred, buffer_);
    else
    {
        commandCompletion.statusCode = 
            transferDataFromBuffer (function.busAddress,
                commandCompletion.wordsTransferred, buffer_);
    }

    return commandCompletion.statusCode == StatusCode::Success;
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