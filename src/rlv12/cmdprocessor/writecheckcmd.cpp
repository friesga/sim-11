#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Write Check command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::writeCheckCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    CondData<u16> comp;
    u16 rlcsValue {0};

    // Verify the unit is available
    if (!unitAvailable (unit))
    {
        // Set spin error
        unit->driveStatus_ |= RLV12::MPR_GS_SpinError;

        // Flag error
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    // Check the validity of cylinder and sector address
    if (!diskAddressOk (unit, rlv12Command))
        return RLV12::CSR_CompositeError | RLV12::CSR_HeaderNotFound | 
               RLV12::CSR_OperationIncomplete;

    // Check for sector overflow
    limitWordCount (rlv12Command);

    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    // The time needed to execute this function is determined by trial
    // and error.
    std::this_thread::sleep_for (std::chrono::milliseconds (10));

    // Guard against drive access while a seek is running
	std::lock_guard<std::mutex> guard{ unit->driveMutex_ };

    // Set position in file to the block to be read
    if (fseek (unit->filePtr_, 
        filePosition (rlv12Command.diskAddress_), SEEK_SET))
    {
        Logger::instance() << "Seek error in writeCheckCmd";
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    size_t numBytes = fread (controller_->dataBuffer_, sizeof (int16_t), 
        rlv12Command.wordCount_, unit->filePtr_);

    if (!ferror (unit->filePtr_))
    {
        // Clear remainder of buffer
        for (size_t index = numBytes; index < rlv12Command.wordCount_; ++index)
            controller_->dataBuffer_[index] = 0;

        // Save wordCount
        size_t numWordsToCheck = rlv12Command.wordCount_;
        int32_t memAddr = rlv12Command.memoryAddress_;

        for (rlv12Command.wordCount_ = 0; 
            rlv12Command.wordCount_ < numWordsToCheck;
            memAddr += 2, ++rlv12Command.wordCount_)
        {
            // Loop through buffer
            comp = controller_->bus_->read (memAddr).valueOr (0);
            if (!comp.hasValue ())
            {
                rlcsValue = RLV12::CSR_CompositeError | 
                    RLV12::CSR_NonExistentMemory;
                break;
            }

            // Check read word with buffer
            // ToDo: Quit for loop when an inequality is detected?
            if (comp != controller_->dataBuffer_[rlv12Command.wordCount_])
                rlcsValue = RLV12::CSR_CompositeError | RLV12::CSR_ReadDataCRC;
        }
    }
    else
    {
        Logger::instance() << "Seek error in writeCheckCmd";
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    updateHeadPosition 
        (CmdProcessor::HeadPositionProcedure::DiskAddressRegister, 
            unit, rlv12Command.wordCount_);

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}