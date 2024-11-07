#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Read Data Without Header Check command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::readDataWithoutHeaderCheckCmd (RL01_02 *unit, 
    RLV12Command &rlv12Command)
{
    u16 rlcsValue {0};

    // Verify the unit is available and no Volume Check condition exists.
    if (!unit->available () || unit->volumeCheck ())
    {
        // EK-RLV12-TD-001 Figure 4-18 states a Operation Incomplete is
        // returned when the Read Data without Header Check command fails.
        return RLV12const::CSR_OperationIncomplete;
    }

    if (RLV12const::getSector (unit->currentDiskAddress_) >= 
            RLV12const::sectorsPerSurface)
	    // Bad sector
	    return RLV12const::CSR_HeaderNotFound;

    // Check for sector overflow
    size_t maxWordCount = 
        (RLV12const::sectorsPerSurface - 
            RLV12const::getSector (unit->currentDiskAddress_)) * 
            RLV12const::wordsPerSector;

    if (rlv12Command.wordCount_ > maxWordCount)
        rlv12Command.wordCount_ = maxWordCount;

    auto [success, numBytes] = unit->readData (rlv12Command,
        controller_->dataBuffer_, RL01_02::HeadPositionProcedure::Increment,
        controller_->dar_);

    if (success)
    {
        // Clear the part of the buffer not filled by the read
        for (size_t index = numBytes; index < rlv12Command.wordCount_; ++index)
            controller_->dataBuffer_[index] = 0;

        // Transfer words in buffer
        BusAddress memAddr = rlv12Command.memoryAddress_;
        for (size_t index = 0; index < rlv12Command.wordCount_;
            memAddr += 2, ++index)
        {
            if (!controller_->bus_->writeWord (memAddr, 
                   controller_->dataBuffer_[index]))
                rlcsValue = RLV12const::CSR_NonExistentMemory;
        }
    }
    else
    {
        Logger::instance() << "Error in readDataWithoutHeaderCheckCmd";
        return RLV12const::CSR_OperationIncomplete;
    }

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}