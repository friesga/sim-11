#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Write Check command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::writeCheckCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    CondData<u16> comp;
    u16 rlcsValue {0};

    // Verify the unit is available
    if (!unit->available ())
    {
        // Set spin error
        unit->driveStatus_ |= RLV12const::MPR_GS_SpinError;

        // Flag error
        return RLV12const::CSR_CompositeError | RLV12const::CSR_OperationIncomplete;
    }

    // Check the validity of cylinder and sector address
    if (!diskAddressOk (unit, rlv12Command))
        return RLV12const::CSR_CompositeError | RLV12const::CSR_HeaderNotFound | 
               RLV12const::CSR_OperationIncomplete;

    // Check for sector overflow
    limitWordCount (rlv12Command);

    auto [success, numBytes] = unit->readData (rlv12Command,
        controller_->dataBuffer_,
        RL01_02::HeadPositionProcedure::DiskAddressRegister, controller_->dar_);

    if (success)
    {
        // Clear remainder of buffer
        for (size_t index = numBytes; index < rlv12Command.wordCount_; ++index)
            controller_->dataBuffer_[index] = 0;

        // Save wordCount
        size_t numWordsToCheck = rlv12Command.wordCount_;
        BusAddress memAddr = rlv12Command.memoryAddress_;

        for (rlv12Command.wordCount_ = 0; 
            rlv12Command.wordCount_ < numWordsToCheck;
            memAddr += 2, ++rlv12Command.wordCount_)
        {
            // Loop through buffer
            comp = controller_->bus_->read (memAddr).valueOr (0);
            if (!comp.hasValue ())
            {
                rlcsValue = RLV12const::CSR_CompositeError | 
                    RLV12const::CSR_NonExistentMemory;
                break;
            }

            // Check read word with buffer
            // ToDo: Quit for loop when an inequality is detected?
            if (comp != controller_->dataBuffer_[rlv12Command.wordCount_])
                rlcsValue = RLV12const::CSR_CompositeError | RLV12const::CSR_ReadDataCRC;
        }
    }
    else
    {
        Logger::instance() << "Error in writeCheckCmd";
        return RLV12const::CSR_CompositeError | RLV12const::CSR_OperationIncomplete;
    }

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}