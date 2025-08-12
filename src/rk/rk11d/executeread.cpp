#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;
using RKTypes::rk05Geometry_;

void RK11D::executeRead (RKTypes::Function function)
{ 
    RKTypes::CommandCompletion commandCompletion {};
    u16 driveId = function.diskAddress.driveSelect;

    if (!driveReady (function))
        return;

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;

    // ToDo: Check for sector overflow

    if (!driveSeek (function, commandCompletion))
        return;

    if (function.rkcs.format)
        commandCompletion = driveReadHeader (function,
            commandCompletion);
    else
        commandCompletion = driveRead (function, commandCompletion);

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
    }

    updateRegisters (function, commandCompletion);
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