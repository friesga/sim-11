#include "rk11d.h"

// ToDo: Pass Function as argument?
void RK11D::executeWrite (RKTypes::Function function)
{
    u16 completion {};
    u16 driveId = function.diskAddress.driveSelect;

    // Check the unit is available
    if (rk05Drives_[driveId]->isReady ())
    {
        // WordCount is 2's complement
        
        // Check wordCount
        // Check the validity of diskAddress
        // Check for sector overflow

        // Transfer memory data from busAddress to internal buffer
        // while adjusting wordCount en BusAddress register

        // Clear to end of block

        // Command RK05 to write data from buffer to disk
        rk05Drives_[driveId]->write (
            DiskAddress {function.diskAddress.sectorAddress,
            function.diskAddress.surface,
            function.diskAddress.cylinderAddress},
            absValueFromTwosComplement (function.wordCount),
            buffer_.get ());

        // Await the result of the execution of the write
        commandCompletionQueue_.waitAndPop (completion);

        // Adjust RKBA, RKWC registers
    }

    // Set controller ready
    setControlReady ();
}

// The word count in the RKWC register is given as a two's complement
// negative number. To be able to use this value as a word counter 
u32 RK11D::absValueFromTwosComplement (u16 value) const
{
    return static_cast<u32> (0200000 - value);
}

void RK11D::dataTransferComplete (u16 wordsTransferred)
{
    commandCompletionQueue_.push (wordsTransferred);
}