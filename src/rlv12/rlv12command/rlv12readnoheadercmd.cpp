#include "rlv12readnoheadercmd.h"
#include "rlv12/rlv12.h"

// Read Data Without Header Check command
void RLV12ReadNoHeaderCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    // Set position in file to the block to be read
    if (!fseek (unit->filePtr_, filePosition(), SEEK_SET))
        return;

    // Read wordCount * 2 bytes; returned is the number of bytes read 
    int32_t numBytes = fread (controller->rlxb_, sizeof (int16_t), wordCount_,
        unit->filePtr_);

    // ToDo: Check error
    if (!ferror (unit->filePtr_))
    {
        // Clear the part of the buffer not filled by the read
        for (size_t index = numBytes; index < wordCount_; ++index)
            controller->rlxb_[index] = 0;

        // Transfer words in buffer
        for (int32_t index = 0, memAddr = memoryAddress_;
            index < wordCount_; memAddr += 2, ++index)
        {
            if (!controller->bus->writeWord (memAddr, 
                controller->rlxb_[index]))
                    controller->rlcs |= RLCS_ERR | RLCS_NXM;
        }
    }
}