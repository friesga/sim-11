#include "cmdprocessor.h"

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
        unit->driveStatus_ |= RLDS_SPE;

        // Flag error
        return RLCS_ERR | RLCS_INCMP;
    }

    // Check the validity of cylinder and sector address
    if (!diskAddressOk (unit, rlv12Command))
        return RLCS_ERR | RLCS_HNF | RLCS_INCMP;

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
        // ToDo: Return I/O error
        return 0;

    size_t numBytes = fread (controller_->rlxb_, sizeof (int16_t), 
        rlv12Command.wordCount_, unit->filePtr_);

    if (!ferror (unit->filePtr_))
    {
        // Clear remainder of buffer
        for (size_t index = numBytes; index < rlv12Command.wordCount_; ++index)
            controller_->rlxb_[index] = 0;

        // Save wordCount
        // ToDo: Rename awc
        int32_t awc = rlv12Command.wordCount_;
        int32_t memAddr = rlv12Command.memoryAddress_;
        for (rlv12Command.wordCount_ = 0; rlv12Command.wordCount_ < awc;
            memAddr += 2, ++rlv12Command.wordCount_)
        {
            // Loop through buffer
            comp = controller_->bus->read (memAddr).valueOr (0);
            if (!comp.hasValue ())
            {
                rlcsValue = RLCS_ERR | RLCS_NXM;
                break;
            }

            // Check read word with buffer
            // ToDo: Quit for loop when an inequality is detected?
            if (comp != controller_->rlxb_[rlv12Command.wordCount_])
                rlcsValue = RLCS_ERR | RLCS_CRC;
        }
    }

    updateHeadPosition 
        (CmdProcessor::HeadPositionProcedure::DiskAddressRegister, 
            unit, rlv12Command.wordCount_);

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}