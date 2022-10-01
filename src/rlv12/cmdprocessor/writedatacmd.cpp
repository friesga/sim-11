#include "cmdprocessor.h"

//
// Perform a Write Data command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::writeDataCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    CondData<u16> tmpValue;
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

    // Revolutional latency is 12.5ms average (EK-RLV-TD-001). 
    // The time needed to execute this function is determined by trial
    // and error.
    std::this_thread::sleep_for (std::chrono::milliseconds (10));

    // Guard against drive access while a seek is running
	std::lock_guard<std::mutex> guard{ unit->driveMutex_ };

    // Set position in file to the block to be written
    if (fseek (unit->filePtr_, 
            filePosition (rlv12Command.diskAddress_), SEEK_SET))
        return RLCS_ERR | RLCS_INCMP;

    if (unit->unitStatus_ & Status::UNIT_RO || 
        unit->rlStatus_ & RlStatus::UNIT_WLK)
    {
        unit->driveStatus_ |= RLDS_WGE;                     
        return RLCS_ERR | RLCS_DRE;
    }

    for (int32_t index = 0, memAddr = rlv12Command.memoryAddress_;
        index < rlv12Command.wordCount_; memAddr += 2, ++index)
    {
        tmpValue = controller_->bus->read (memAddr).valueOr (0);
        if (!tmpValue.hasValue ())
        {
            rlcsValue = RLCS_ERR | RLCS_NXM;
            // Set adj xfer length
            rlv12Command.wordCount_ -= index;
            break;
        }
        controller_->rlxb_[index] = tmpValue;
    }

    // Any xfer?
    if (rlv12Command.wordCount_)
    {
        // Clear to end of block
        // ToDo: Rename awc
        size_t awc = (rlv12Command.wordCount_ + 
            (RL_NUMWD - 1)) & ~(RL_NUMWD - 1);
        for (size_t index = rlv12Command.wordCount_; index < awc; ++index)
            controller_->rlxb_[index] = 0;

        fwrite (controller_->rlxb_, sizeof (int16_t), awc, unit->filePtr_);
        // ToDo: Handle possible fwrite error
        // err = ferror (filePtr_);
    }

    updateHeadPosition 
        (CmdProcessor::HeadPositionProcedure::DiskAddressRegister, 
            unit, rlv12Command.wordCount_);

    finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}