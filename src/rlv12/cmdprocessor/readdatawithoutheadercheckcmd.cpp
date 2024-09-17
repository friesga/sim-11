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

    // Verify the unit is available
    if (!unit->available ())
    {
        // Set spin error
        unit->driveStatus_ |= RLV12::MPR_GS_SpinError;

        // Flag error
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    if (RLV12::getSector (unit->currentDiskAddress_) >= 
            RLV12::sectorsPerSurface)
	    // Bad sector
	    return RLV12::CSR_CompositeError | RLV12::CSR_HeaderNotFound;

    // Check for sector overflow
    size_t maxWordCount = 
        (RLV12::sectorsPerSurface - 
            RLV12::getSector (unit->currentDiskAddress_)) * 
            RLV12::wordsPerSector;

    if (rlv12Command.wordCount_ > maxWordCount)
        rlv12Command.wordCount_ = maxWordCount;

    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    alarmClock_.sleepFor (std::chrono::microseconds (12500));

    auto [success, numBytes] = unit->readData (rlv12Command,
        controller_->dataBuffer_);

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
                rlcsValue = RLV12::CSR_CompositeError | 
                    RLV12::CSR_NonExistentMemory;
        }
    }
    else
    {
        Logger::instance() << "Error in readDataWithoutHeaderCheckCmd";
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    updateHeadPosition 
        (CmdProcessor::HeadPositionProcedure::Increment, 
         unit, rlv12Command.wordCount_);

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}