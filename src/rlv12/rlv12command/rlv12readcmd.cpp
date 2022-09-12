#include "rlv12readcmd.h"
#include "rlv12/rlv12.h"
 
void RLV12ReadCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    // Read Data or Read Data Without Header Check command
    // Read wordCount * 2 bytes; returned is the number of bytes read 
    size_t numBytes = fread (controller->rlxb_, sizeof (int16_t), wordCount_,
        unit->filePtr_);

    // ToDo: Check error
    if (!ferror (unit->filePtr_))
    {
        // Clear the part of the buffer not filled by the read
        for (size_t index = numBytes; index < wordCount_; ++index)
            controller->rlxb_[index] = 0;

        // Transfer words in buffer
        for (size_t index = 0; index < wordCount_; memoryAddress_ += 2, ++index)
            if (!controller->bus->writeWord (memoryAddress_, 
                controller->rlxb_[index]))
                    controller->rlcs |= RLCS_ERR | RLCS_NXM;
    }
}