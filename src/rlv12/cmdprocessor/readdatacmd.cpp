#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Read Data command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::readDataCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    u16 rlcsValue {0};

    // Verify the unit is available, the cylinder and sector address
    // are valid and no Volume Check condition exists.
    if (!unit->available () || 
        !diskAddressOk (unit, rlv12Command) ||
        unit->volumeCheck ())
    {
        // EK-RLV12-TD-001 Figure 4-16 states a Header Not Found error and
        // Operation Incomplete are returned when the Read Data command fails.
        return RLV12const::CSR_OperationIncomplete |
            RLV12const::CSR_HeaderNotFound;
    }

    // Check for sector overflow
    limitWordCount (rlv12Command);

    auto [success, numBytes] = unit->readData (rlv12Command,
        controller_->dataBuffer_,
        RL01_02::HeadPositionProcedure::DiskAddressRegister, controller_->dar_);

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
            if (!controller_->bus_->dmaWrite (memAddr, 
                    controller_->dataBuffer_[index]))
                rlcsValue = RLV12const::CSR_NonExistentMemory;
        }
    }
    else
    {
        Logger::instance() << "Error in readDataCmd";
        return RLV12const::CSR_OperationIncomplete;
    }

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}