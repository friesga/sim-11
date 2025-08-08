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

    if (function.rkcs.format)
        driveReadHeader (function, commandCompletion);
    else
    {
        if (!driveSeek (function, commandCompletion))
            return;

        commandCompletion = driveRead (function, commandCompletion);
    }

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