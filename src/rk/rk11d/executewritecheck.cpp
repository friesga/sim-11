#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>

using std::out_of_range;
using RKTypes::rk05Geometry_;

// The Write Check function is used to compare the contents of memory to the
// contents of a continuous block of data on a disk cartridge. The controller
// first performs a Seek function, just as for a Write function, and then
// reads and checks the next header word. If the cylinder address is correct,
// the controller waits for SC = SA, then begins reading the rest of the
// sector (Data and Checksum) while performing BUS NPR transfers for each data
// word. Each data word from the disk drive is compared, bit by bit, with
// memory data from the Unibus. The disk drive checksum, in turn, is compared
// with the checksum calculated by the controller. If any bit is found to be
// in error, RKER 00 (Write Check Error) is set. Controller reaction is then
// determined by RKCS 06 (IDE) and RKCS 08 (SSE). The Write Check function may
// be performed on a short sector (less than 256 data words) as long as the
// number of words write checked is equal to the number of words previously
// written into the sector. (EK-RK11D-MM-002 par. 1.3.2.7)
//
// ToDo: Value of RKBA and RKWC probably has to be adapted to the result of
// the data comparison.
//
void RK11D::executeWriteCheck (RKTypes::Function function)
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

    // Check for sector overflow

    // Stop possible running hardware poll
    pollEventQueue_.push (StopPoll {});

    // Command RK05 to read data from disk to buffer
    rk05Drives_[driveId]->read (
        DiskAddress {function.diskAddress.sectorAddress,
        function.diskAddress.surface,
        function.diskAddress.cylinderAddress},
        absValueFromTwosComplement (function.wordCount),
        buffer_.get ());

    // Await the result of the execution of the read
    commandCompletionQueue_.waitAndPop (commandCompletion);

    if (commandCompletion.wordsTransferred <
            absValueFromTwosComplement (function.wordCount))
        setError ([&] {rker_.overrun = 1; });

    // In the normal case wordCount words starting at the address in the
    // RKBA are compared with the buffer contents. Setting the RKCS IBA bit
    // inhbits the RKBA from incrementing during the transfer function. This
    // means that the buffer is compared with a pattern at the memory address
    // indicated by the RKBA.
    StatusCode status {};
    if (function.rkcs.inhibitIncrementingRKBA)
    {
        status = comparePatternWithBuffer (function.busAddress,
            commandCompletion.wordsTransferred, buffer_);
    }
    else
    {
        status = compareDataWithBuffer (function.busAddress,
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
    rkdb_ = buffer_[commandCompletion.wordsTransferred - 1];
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

    if (status != StatusCode::Success)
        setError ([&] {rker_.writeCheckError = 1; });
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