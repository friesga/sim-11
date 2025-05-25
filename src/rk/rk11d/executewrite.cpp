#include "rk11d.h"

// ToDo: Pass Function as argument?
void RK11D::executeWrite (RKTypes::Function function)
{
    u16 wordsWritten {};
    u16 driveId = function.diskAddress.driveSelect;

    // Check the drive is ready
    if (!rk05Drives_[driveId]->isReady ())
    {
        setError ([&] {rker_.driveError = 1; });
        setControlReady ();
        return;
    }

    // Check validity of the function's parameters
    if (!functionParametersOk (function))
        return;

    // Check for sector overflow

    // Transfer memory data from busAddress to internal buffer
    // while adjusting wordCount en BusAddress register
    if (transferDataToBuffer (function.busAddress, function.wordCount,
        buffer_) != StatusCode::Success)
    {
        // Set error condition
        return;
    }

    // Clear to end of block

    // Command RK05 to write data from buffer to disk
    rk05Drives_[driveId]->write (
        DiskAddress {function.diskAddress.sectorAddress,
        function.diskAddress.surface,
        function.diskAddress.cylinderAddress},
        absValueFromTwosComplement (function.wordCount),
        buffer_.get ());

    // Await the result of the execution of the write
    commandCompletionQueue_.waitAndPop (wordsWritten);

    // Check for errors

    // Adjust RKBA, RKWC registers
    rkwc_ += wordsWritten;
    rkba_ += wordsWritten;

    // Else indicate error

    // Set controller ready
    setControlReady ();
}

// The word count in the RKWC register is given as a two's complement
// negative number. To be able to use this value as a word counter 
u32 RK11D::absValueFromTwosComplement (u16 value) const
{
    return static_cast<u32> (0200000 - value);
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


void RK11D::dataTransferComplete (u16 wordsTransferred)
{
    commandCompletionQueue_.push (wordsTransferred);
}