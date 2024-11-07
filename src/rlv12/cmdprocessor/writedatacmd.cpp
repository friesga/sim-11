#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Write Data command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::writeDataCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    CondData<u16> tmpValue;
    u16 rlcsValue {0};

    // Check the unit is available and no Volume Check condition exists.
    if (!unit->available () || unit->volumeCheck ())
    {
        // EK-RLV12-TD-001 Figure 4-12 states a Header Not Found error and
        // Operation Incomplete are returned when the Write Data command fails.
        return RLV12const::CSR_CompositeError |
            RLV12const::CSR_OperationIncomplete |
            RLV12const::CSR_HeaderNotFound;
    }

    // Check the validity of cylinder and sector address
    if (!diskAddressOk (unit, rlv12Command))
        return RLV12const::CSR_CompositeError | RLV12const::CSR_HeaderNotFound | 
               RLV12const::CSR_OperationIncomplete;

    // Check for sector overflow
    limitWordCount (rlv12Command);

    BusAddress memAddr = rlv12Command.memoryAddress_;
    for (size_t index = 0; index < rlv12Command.wordCount_;
        memAddr += 2, ++index)
    {
        tmpValue = controller_->bus_->read (memAddr).valueOr (0);
        if (!tmpValue.hasValue ())
        {
            rlcsValue = RLV12const::CSR_CompositeError | 
                RLV12const::CSR_NonExistentMemory;
            // Set adj xfer length
            rlv12Command.wordCount_ -= index;
            break;
        }
        controller_->dataBuffer_[index] = tmpValue;
    }

    // Any xfer?
    if (rlv12Command.wordCount_)
    {
        // Clear to end of block
        size_t numWordsToWrite = (rlv12Command.wordCount_ + 
            (RLV12const::wordsPerSector - 1)) & ~(RLV12const::wordsPerSector - 1);

        for (size_t index = rlv12Command.wordCount_; 
                index < numWordsToWrite; ++index)
            controller_->dataBuffer_[index] = 0;

        auto [success, numBytes] = unit->writeData (rlv12Command,
            controller_->dataBuffer_, numWordsToWrite,
            RL01_02::HeadPositionProcedure::DiskAddressRegister,
            controller_->dar_);

        if (!success)
        {
            Logger::instance() << "Write error in writeDataCmd";
            return RLV12const::CSR_CompositeError | RLV12const::CSR_OperationIncomplete;
        }
    }

    // Catch erors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}