#include "rk11d.h"
#include "rk/include/rktypes.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>

using std::out_of_range;
using std::ranges::all_of;

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
    RKTypes::CommandCompletion commandCompletion {};

    all_of (writeCheckFunction_, [&] (auto& f)
        { return f (function, commandCompletion); });
}

bool RK11D::compareBufferWithMemory (RKTypes::Function function,
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