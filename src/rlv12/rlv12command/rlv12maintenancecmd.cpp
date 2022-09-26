#include "rlv12maintenancecmd.h"
#include "rlv12/rlv12.h"

#include <mutex>

using std::mutex;
using std::lock_guard;
 
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
void RLV12MaintenanceCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    u32  memoryAddress;

    // This command is a NOP on the RL11 controller
    if (controller->rlType_ == RlConfig::RLType::RL11)
    {
        controller->setDone (0);
        return;
    }

    // The VRLBC0 diagnostic expects a reaction on a 
    // Maintenance command between 155 and 650 milli-
    // seconds. This time is determined by executing a
    // number of instructions. As the emulated instructions
    // are not timed (yet) the reaction time will vary per
    // host CPU and has to be determined by trial 
    // and error.

    // ToDo: Add wait interval
    // timer.start (&unit, std::chrono::milliseconds (20));

    // Guard against controller register access
	lock_guard<mutex> guard{ controller->controllerMutex_ };


    // Test 1: Check internal logic
    controller->rlda = (controller->rlda & ~0377) | 
        ((controller->rlda + 1) & 0377);

    // Test 2: Check internal logic 
    controller->rlda = (controller->rlda & ~0377) | 
        ((controller->rlda + 1) & 0377);

    // Test 3: Check DMA transfers
    // Get memory address from BAR, CSR and possibly BAE registers
    memoryAddress = controller->memAddrFromRegs ();
                                                            
    if(trc.flags & TRACEF_RLV12)
         TRACERLV12Registers (&trc, "Maintenance", controller->rlcs,
             controller->rlba, controller->rlda, controller->rlmpr,
             controller->rlbae);

    // Must be exactly -511
    if (controller->rlmpr != 0177001)
    {                              
        // HNF error
        // controller->rlcs |= RLCS_ERR | RLCS_HNF;
        controller->setDone (RLCS_ERR | RLCS_HNF);
        return;
    }

    // Transfer 256 words to FIFO
    // The success of the bus->read() operation is not tested as on failures
    // bus->read() will generate a bus error trap.
    for (size_t wordCount = 0; wordCount < 256; ++wordCount)
    {
        CondData<u16> value = controller->bus->read (memoryAddress);
        if (value.hasValue())
            controller->rlxb_[wordCount] = value;
        else
        {
            controller->rlcs |= RLCS_ERR | RLCS_NXM;
            break;
        }
        memoryAddress += 2;
        controller->rlmpr++;
    }

    // Transfer 255 words from FIFO to the original memory address
    // plus 512 (using memoryAddress from the previous loop).
    // Just as for the bus->read() operation, the success of the
    // bus->writeWord() operation is not tested as on failures it will
    // generate a bus error trap.
    for (size_t wordCount = 0; wordCount < 255; wordCount++)
    {
        controller->bus->writeWord (memoryAddress, 
            controller->rlxb_[wordCount]);
        memoryAddress += 2;
        controller->rlmpr++;
    }

    // Update DAR and bus address in BA and BAE
    controller->rlda = (controller->rlda & ~0377) |
        ((controller->rlda + 1) & 0377);

    // Load BAR, CSR and possibly BAE with the current address
    controller->memAddrToRegs (memoryAddress);
    
    // Test 4: Check the CRC of (DAR + 3)
    u16 word = static_cast<u16> (controller->rlda);
    controller->rlxb_[0] = controller->calcCRC (1, &word);
    controller->rlda = (controller->rlda & ~0377) | 
        ((controller->rlda + 1) & 0377);

    // Test 5: Check the CRC of (DAR + 4)
    word = static_cast<u16> (controller->rlda);
    controller->rlxb_[1] = controller->calcCRC (1, &word);
    controller->rlda = (controller->rlda & ~0377) | 
        ((controller->rlda + 1) & 0377);

    // Test 6: Check the CRC of (CRC of DAR + 4)
    word = controller->rlxb_[1];
    controller->rlxb_[1] = controller->calcCRC (1, &word);
    controller->rlmpr = controller->rlxb_[0];
    controller->rlmpr1 = controller->rlxb_[1];
    controller->rlda = (controller->rlda & ~0377) | 
        ((controller->rlda + 1) & 0377);

    controller->setDone (0);
}

// No action to finish this command
void RLV12MaintenanceCmd::finish (RLV12 *controller, RL01_2 *unit)
{}