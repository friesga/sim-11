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
    u32  memoryAddress;
    u16 rlcsValue {0};

    // The VRLBC0 diagnostic expects a reaction on a Maintenance command
    // between 155 and 650 milliseconds. This time is determined by executing
    // a number of instructions. As the emulated instructions are not timed
    // (yet) the reaction time will vary per host CPU and has to be determined
    // by trial and error.
    std::this_thread::sleep_for(std::chrono::milliseconds (20));

    // This command is a NOP on the RL11 controller
    if (controller_->rlType_ == RlConfig::RLType::RL11)
        return 0;

    // The VRLBC0 diagnostic expects a reaction on a 
    // Maintenance command between 155 and 650 milli-
    // seconds. This time is determined by executing a
    // number of instructions. As the emulated instructions
    // are not timed (yet) the reaction time will vary per
    // host CPU and has to be determined by trial 
    // and error.

    // Test 1: Check internal logic
    controller_->rlda = (controller_->rlda & ~0377) | 
        ((controller_->rlda + 1) & 0377);

    // Test 2: Check internal logic 
    controller_->rlda = (controller_->rlda & ~0377) | 
        ((controller_->rlda + 1) & 0377);

    // Test 3: Check DMA transfers
    // Get memory address from BAR, CSR and possibly BAE registers
    memoryAddress = controller_->memAddrFromRegs ();
                                                            
    if(trc.flags & TRACEF_RLV12)
         TRACERLV12Registers (&trc, "Maintenance", controller_->rlcs,
             controller_->rlba, controller_->rlda, controller_->rlxb_[0],
             controller_->rlbae);

    // Must be exactly -511
    if (controller_->wordCounter_ != 0177001)
    {
        // HNF error
        return RLCS_ERR | RLCS_HNF | RLCS_INCMP;
    }

    // Transfer 256 words to FIFO
    // The success of the bus->read() operation is not tested as on failures
    // bus->read() will generate a bus error trap.
    for (size_t wordCount = 0; wordCount < 256; ++wordCount)
    {
        CondData<u16> value = controller_->bus->read (memoryAddress);
        if (value.hasValue())
            controller_->rlxb_[wordCount] = value;
        else
        {
            rlcsValue = RLCS_ERR | RLCS_NXM;
            break;
        }
        memoryAddress += 2;
        ++controller_->wordCounter_;
    }

    // Transfer 255 words from FIFO to the original memory address
    // plus 512 (using memoryAddress from the previous loop).
    // Just as for the bus->read() operation, the success of the
    // bus->writeWord() operation is not tested as on failures it will
    // generate a bus error trap.
    for (size_t wordCount = 0; wordCount < 255; wordCount++)
    {
        controller_->bus->writeWord (memoryAddress, 
            controller_->rlxb_[wordCount]);
        memoryAddress += 2;
        ++controller_->wordCounter_;
    }

    // Update DAR and bus address in BA and BAE
    controller_->rlda = (controller_->rlda & ~0377) |
        ((controller_->rlda + 1) & 0377);

    // Load BAR, CSR and possibly BAE with the current address
    controller_->memAddrToRegs (memoryAddress);
    
    // Test 4: Check the CRC of (DAR + 3)
    u16 word = static_cast<u16> (controller_->rlda);
    controller_->rlxb_[0] = controller_->calcCRC (1, &word);
    controller_->rlda = (controller_->rlda & ~0377) | 
        ((controller_->rlda + 1) & 0377);

    // Test 5: Check the CRC of (DAR + 4)
    word = static_cast<u16> (controller_->rlda);
    controller_->rlxb_[1] = controller_->calcCRC (1, &word);
    controller_->rlda = (controller_->rlda & ~0377) | 
        ((controller_->rlda + 1) & 0377);

    // Test 6: Check the CRC of (CRC of DAR + 4)
    word = controller_->rlxb_[1];
    controller_->rlxb_[1] = controller_->calcCRC (1, &word);
    controller_->rlda = (controller_->rlda & ~0377) | 
        ((controller_->rlda + 1) & 0377);

    return rlcsValue;
}