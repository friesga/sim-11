#include "rk11d.h"

// The Write Check function is used to compare the contents of memory to the
// contents of a continuous block of data on a disk cartridge. The controller
// first performs a Seek function, just as for a Write function, and then
// reads and checks the next header word.If the cylinder address is correct,
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
    u16 wordsRead {};
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
    commandCompletionQueue_.waitAndPop (wordsRead);

    // Adjust RKBA, RKWC registers
    rkwc_ += wordsRead;
    rkba_ += wordsRead;

    if (wordsRead < absValueFromTwosComplement (function.wordCount))
        setError ([&] {rker_.overrun = 1; });

    // Transfer words in buffer
    StatusCode status = 
        compareDataWithBuffer (function.busAddress, wordsRead, buffer_);

    if (status != StatusCode::Success)
        setError ([&] {rker_.writeCheckError = 1; });

    // Set controller ready
    setControlReady ();
}

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