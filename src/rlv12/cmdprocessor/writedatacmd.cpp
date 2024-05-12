#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Write Data command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::writeDataCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    CondData<u16> tmpValue;
    u16 rlcsValue {0};

    // Check the unit is available
    if (!unitAvailable (unit))
    {
        // Set spin error
        unit->driveStatus_ |= RLV12::MPR_GS_SpinError;

        // Flag error
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    // Check the unit is not write-protected
    if (unit->unitStatus_ & Status::UNIT_RO || 
        unit->rlStatus_ & RlStatus::UNIT_WLK)
    {
        unit->driveStatus_ |= RLV12::MPR_GS_WriteGateError;                     
        return RLV12::CSR_CompositeError;
    }

    // Check the validity of cylinder and sector address
    if (!diskAddressOk (unit, rlv12Command))
        return RLV12::CSR_CompositeError | RLV12::CSR_HeaderNotFound | 
               RLV12::CSR_OperationIncomplete;

    // Check for sector overflow
    limitWordCount (rlv12Command);

    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    alarmClock_.sleepFor (std::chrono::microseconds (12500));

    // Guard against drive access while a seek is running
	std::lock_guard<std::mutex> guard{ unit->driveMutex_ };

    // Set position in file to the block to be written
    if (fseek (unit->filePtr_, 
            filePosition (rlv12Command.diskAddress_), SEEK_SET))
    {
        Logger::instance() << "Seek error in writeDataCmd";
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    BusAddress memAddr = rlv12Command.memoryAddress_;
    for (size_t index = 0; index < rlv12Command.wordCount_;
        memAddr += 2, ++index)
    {
        tmpValue = controller_->bus_->read (memAddr).valueOr (0);
        if (!tmpValue.hasValue ())
        {
            rlcsValue = RLV12::CSR_CompositeError | 
                RLV12::CSR_NonExistentMemory;
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
            (RLV12::wordsPerSector - 1)) & ~(RLV12::wordsPerSector - 1);

        for (size_t index = rlv12Command.wordCount_; 
                index < numWordsToWrite; ++index)
            controller_->dataBuffer_[index] = 0;

        fwrite (controller_->dataBuffer_, sizeof (int16_t), numWordsToWrite, unit->filePtr_);
        
        if (ferror (unit->filePtr_))
        {
            Logger::instance() << "Write error in writeDataCmd";
            return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
        }
    }

    updateHeadPosition 
        (CmdProcessor::HeadPositionProcedure::DiskAddressRegister, 
            unit, rlv12Command.wordCount_);

    // Catch erors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}