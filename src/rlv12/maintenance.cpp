#include "rlv12.h"

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
void RLV12::maintenance()
{
    u32  memoryAddress;

    // Test 1: Check internal logic
    rlda = (rlda & ~0377) | ((rlda + 1) & 0377);

    // Test 2: Check internal logic 
    rlda = (rlda & ~0377) | ((rlda + 1) & 0377);

    // Test 3: Check DMA transfers
    // Get memory address from BAR, CSR and possibly BAE registers
    memoryAddress = memAddrFromRegs ();
                                                            
    if(trc.flags & TRACEF_RLV12)
         TRACERLV12Registers (&trc, "Maintenance", rlcs, rlba, rlda, rlmpr, rlbae);

    // Must be exactly -511
    if (rlmpr != 0177001)
    {                              
        // HNF error
        // ToDo: The status should be returned via SetDone() call?
        rlcs |= RLCS_ERR | RLCS_HDE;                    
        return;
    }

    // Transfer 256 words to FIFO
    // The success of the bus->read() operation is not tested as on failures
    // bus->read() will generate a bus error trap.
    for (size_t wordCount = 0; wordCount < 256; ++wordCount)
    {
        // rlxb_[wordCount] = bus->read (memoryAddress);
        CondData<u16> value = bus->read (memoryAddress);
        if (value.hasValue())
            rlxb_[wordCount] = value;
        else
        {
            rlcs |= RLCS_ERR | RLCS_NXM;
            break;
        }
        memoryAddress += 2;
        rlmpr++;
    }

    // Transfer 255 words from FIFO to the original memory address
    // plus 512 (using memoryAddress from the previous loop).
    // Just as for the bus->read() operation, the success of the
    // bus->writeWord() operation is not tested as on failures it will
    // generate a bus error trap.
    for (size_t wordCount = 0; wordCount < 255; wordCount++)
    {
        bus->writeWord (memoryAddress, rlxb_[wordCount]);
        memoryAddress += 2;
        rlmpr++;
    }

    // Update DAR and bus address in BA and BAE
    rlda = (rlda & ~0377) | ((rlda + 1) & 0377);

    // Load BAR, CSR and possibly BAE with the current address
    memAddrToRegs (memoryAddress);
    
    // Test 4: Check the CRC of (DAR + 3)
    u16 word = static_cast<u16> (rlda);
    rlxb_[0] = calcCRC (1, &word);
    rlda = (rlda & ~0377) | ((rlda + 1) & 0377);

    // Test 5: Check the CRC of (DAR + 4)
    word = static_cast<u16> (rlda);
    rlxb_[1] = calcCRC (1, &word);
    rlda = (rlda & ~0377) | ((rlda + 1) & 0377);

    // Test 6: Check the CRC of (CRC of DAR + 4)
    word = rlxb_[1];
    rlxb_[1] = calcCRC (1, &word);
    rlmpr = rlxb_[0];
    rlmpr1 = rlxb_[1];
    rlda = (rlda & ~0377) | ((rlda + 1) & 0377);
}