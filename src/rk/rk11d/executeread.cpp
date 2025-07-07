#include "rk11d.h"

void RK11D::executeRead (RKTypes::Function function)
{ 
    u16 wordsRead {};
    u16 driveId = function.diskAddress.driveSelect;

    // Check the drive is ready
    if (!rk05Drives_[driveId]->isReady ())
    {
        setError ([&] {rker_.driveError = 1;});
        return;
    }

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;

    // ToDo: Check for sector overflow

    // Stop possible running hardware poll
    pollEventQueue_.push (StopPoll {});

    // Get the parameters ready for the read() or readHeader() call
    // to avoid code duplication
    DiskAddress diskAddress
    {
         function.diskAddress.sectorAddress,
         function.diskAddress.surface,
         function.diskAddress.cylinderAddress
    };
    u32 wordCount = absValueFromTwosComplement (function.wordCount);

    // Either read the header of the given sector(s) in Format Mode or
    // the sector(s) itself/themselves.
    if (function.rkcs.format)
        rk05Drives_[driveId]->readHeader (diskAddress, wordCount,
            buffer_.get ());
    else
        rk05Drives_[driveId]->read (diskAddress, wordCount, buffer_.get ());

    // Await the result of the execution of the read
    commandCompletionQueue_.waitAndPop (wordsRead);

    // Adjust RKBA, RKWC registers
    rkwc_ += wordsRead;
    rkba_ += wordsRead;


    // Clear the part of the buffer not filled by the read


    // Transfer words in buffer
    transferDataFromBuffer (function.busAddress, wordsRead, buffer_);

    if (wordsRead < absValueFromTwosComplement (function.wordCount))
        setError ([&] {rker_.overrun = 1; });
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