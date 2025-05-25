#include "rk11d.h"

void RK11D::executeRead (RKTypes::Function function)
{ 
    u16 wordsRead {};
    u16 driveId = function.diskAddress.driveSelect;

    // Check the drive is ready
    if (!rk05Drives_[driveId]->isReady ())
    {
        setError ([&] {rker_.driveError = 1;});
        setControlReady ();
        return;
    }

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;

    // Check for sector overflow

    // Command RK05 to read data from disk to buffer
    rk05Drives_[driveId]->read (
        DiskAddress {function.diskAddress.sectorAddress,
        function.diskAddress.surface,
        function.diskAddress.cylinderAddress},
        absValueFromTwosComplement (function.wordCount),
        buffer_.get ());

    // Await the result of the execution of the read
    commandCompletionQueue_.waitAndPop (wordsRead);


    // Clear the part of the buffer not filled by the read

    // Transfer words in buffer
    transferDataFromBuffer (function.busAddress, wordsRead, buffer_);

    // Adjust RKBA, RKWC registers
    rkwc_ += wordsRead;
    rkba_ += wordsRead;

    // Else indicate error

    // Set controller ready
    setControlReady ();
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