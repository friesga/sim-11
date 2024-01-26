#include "cmdprocessor.h"

//
// Perform the maintenance function of the RLV1x. This function allows
// the RLV1[12] to perform a self-test operation. The maintenance function
// is used to test the controller and may be executed with or without a
// disk drive attached. The function performs six internal tests. This is
// fully described on pages 4-14 and 4-15 of EK-RL012-UG-006.
// 
// Note that the description of this in EK-RLV12-UG-002 (p.5-3) contains a
// typo, the octal representation for -511 (0117701) is incorrect.
//
u16 CmdProcessor::maintenanceCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    BusAddress  memoryAddress {0};
    u16 rlcsValue {0};

    // The VRLBC0 diagnostic expects a reaction on a Maintenance command
    // between 155 and 650 milliseconds. This time is determined by executing
    // a number of instructions. As the emulated instructions are not timed
    // (yet) the reaction time will vary per host CPU and has to be determined
    // by trial and error.
    std::this_thread::sleep_for(std::chrono::milliseconds (55));

    // This command is a NOP on the RL11 controller
    if (controller_->rlType_ == RLConfig::RLType::RL11)
        return 0;

    // Test 1: Check internal logic
    controller_->dar_ = (controller_->dar_ & ~0377) | 
        ((controller_->dar_ + 1) & 0377);

    // Test 2: Check internal logic 
    controller_->dar_ = (controller_->dar_ & ~0377) | 
        ((controller_->dar_ + 1) & 0377);

    // Test 3: Check DMA transfers
    // Get memory address from BAR, CSR and possibly BAE registers
    memoryAddress = controller_->memAddrFromRegs ();
                                                            
    trace.rlv12Registers ("Maintenance", controller_->csr_,
        controller_->bar_, controller_->dar_,
        controller_->dataBuffer_[0], controller_->bae_);

    // Must be exactly -511
    if (controller_->wordCounter_ != 0177001)
    {
        return RLV12::CSR_CompositeError | RLV12::CSR_HeaderNotFound | 
            RLV12::CSR_OperationIncomplete;
    }

    // Transfer 256 words to FIFO
    for (size_t wordCount = 0; wordCount < 256; ++wordCount)
    {
        CondData<u16> value = controller_->bus_->read (memoryAddress);
        if (value.hasValue())
            controller_->dataBuffer_[wordCount] = value;
        else
        {
            rlcsValue = RLV12::CSR_CompositeError | RLV12::CSR_NonExistentMemory;
            break;
        }
        memoryAddress += 2;
        ++controller_->wordCounter_;
    }

    // Transfer 255 words from FIFO to the original memory address
    // plus 512 (using memoryAddress from the previous loop).
    for (size_t wordCount = 0; wordCount < 255; wordCount++)
    {
        if (!controller_->bus_->writeWord (memoryAddress, 
            controller_->dataBuffer_[wordCount]))
        {
            rlcsValue = RLV12::CSR_CompositeError | RLV12::CSR_NonExistentMemory;
            break;
        }

        memoryAddress += 2;
        ++controller_->wordCounter_;
    }

    // Update DAR and bus address in BA and BAE
    controller_->dar_ = (controller_->dar_ & ~0377) |
        ((controller_->dar_ + 1) & 0377);

    // Load BAR, CSR and possibly BAE with the current address
    controller_->memAddrToRegs (memoryAddress);
    
    // Test 4: Check the CRC of (DAR + 3)
    u16 word = static_cast<u16> (controller_->dar_);
    controller_->dataBuffer_[0] = controller_->calcCRC (1, &word);
    controller_->dar_ = (controller_->dar_ & ~0377) | 
        ((controller_->dar_ + 1) & 0377);

    // Test 5: Check the CRC of (DAR + 4)
    word = static_cast<u16> (controller_->dar_);
    controller_->dataBuffer_[1] = controller_->calcCRC (1, &word);
    controller_->dar_ = (controller_->dar_ & ~0377) | 
        ((controller_->dar_ + 1) & 0377);

    // Test 6: Check the CRC of (CRC of DAR + 4)
    word = controller_->dataBuffer_[1];
    controller_->dataBuffer_[1] = controller_->calcCRC (1, &word);
    controller_->dar_ = (controller_->dar_ & ~0377) | 
        ((controller_->dar_ + 1) & 0377);

    return rlcsValue;
}