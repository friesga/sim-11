#include "cmdprocessor.h"

//
// Perform a Read Data Without Header Check command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::readDataWithoutHeaderCheckCmd (RL01_2 *unit, 
    RLV12Command &rlv12Command)
{
    u16 rlcsValue {0};

    // Verify the unit is available
    if (!unitAvailable (unit))
    {
        // Set spin error
        unit->driveStatus_ |= RLDS_SPE;

        // Flag error
        return RLCS_ERR | RLCS_INCMP;
    }

    if (getSector (unit->currentDiskAddress_) >= RL_NUMSC)
	    // Bad sector
	    return RLCS_ERR | RLCS_HNF;

    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    // The time needed to execute this function is determined by trial
    // and error.
    std::this_thread::sleep_for (std::chrono::milliseconds (10));

    // Guard against drive access while a seek is running
	std::lock_guard<std::mutex> guard{ unit->driveMutex_ };

    // Set position in file to the block to be read
    if (fseek (unit->filePtr_, filePosition (rlv12Command.diskAddress_),
            SEEK_SET))
        // ToDo: Return I/O error
        return 0;

    // Read wordCount * 2 bytes; returned is the number of bytes read 
    int32_t numBytes = fread (controller_->rlxb_, sizeof (int16_t), 
        rlv12Command.wordCount_, unit->filePtr_);

    // ToDo: Check error
    if (!ferror (unit->filePtr_))
    {
        // Clear the part of the buffer not filled by the read
        for (size_t index = numBytes; index < rlv12Command.wordCount_; ++index)
            controller_->rlxb_[index] = 0;

        // Transfer words in buffer
        for (int32_t index = 0, memAddr = rlv12Command.memoryAddress_;
            index < rlv12Command.wordCount_; memAddr += 2, ++index)
        {
            if (!controller_->bus->writeWord (memAddr, 
                   controller_->rlxb_[index]))
                rlcsValue = RLCS_ERR | RLCS_NXM;
        }
    }

    updateHeadPosition 
        (CmdProcessor::HeadPositionProcedure::Increment, 
         unit, rlv12Command.wordCount_);

    finishDataTransferCmd (unit, rlv12Command);

    return 0;
}