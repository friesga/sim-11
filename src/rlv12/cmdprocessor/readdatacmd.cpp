#include "cmdprocessor.h"
#include "logger/logger.h"

//
// Perform a Read Data command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::readDataCmd (RL01_02 *unit, RLV12Command &rlv12Command)
{
    u16 rlcsValue {0};

    // Verify the unit is available
    if (!unit->available ())
    {
        // Set spin error and return OPI
        unit->driveStatus_ |=  RLV12::MPR_GS_SpinError;
        return RLV12::CSR_OperationIncomplete;
    }

    // Check the validity of cylinder and sector address
    if (!diskAddressOk (unit, rlv12Command))
        return RLV12::CSR_HeaderNotFound | RLV12::CSR_OperationIncomplete;

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
            if (!controller_->bus_->writeWord (memAddr, 
                    controller_->dataBuffer_[index]))
                rlcsValue = RLV12::CSR_CompositeError | 
                    RLV12::CSR_NonExistentMemory;
        }
    }
    else
    {
        Logger::instance() << "Error in readDataCmd";
        return RLV12::CSR_CompositeError | RLV12::CSR_OperationIncomplete;
    }

    // Catch errors together in rlcsValue
    rlcsValue |= finishDataTransferCmd (unit, rlv12Command);

    return rlcsValue;
}