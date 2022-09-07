#include "rlv12readnoheadercmd.h"
#include "rlv12/rlv12.h"

void RLV12ReadNoHeaderCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    // Read Data or Read Data Without Header Check command
    // Read wordCount * 2 bytes; returned is the number of words read 
    // ToDo: Rename i
    int32_t i = fread (controller->rlxb_, sizeof (int16_t), wordCount_,
        unit->filePtr_);

    // ToDo: Check error
    if (!ferror (unit->filePtr_))
    {
        // Clear the part of the buffer not filled by the read
        for (; i < wordCount_; i++)
            controller->rlxb_[i] = 0;

        // Transfer words in buffer
        for (i = 0; i < wordCount_; memoryAddress_ += 2, ++i)
            if (!controller->bus->writeWord (memoryAddress_, 
                controller->rlxb_[i]))
                    controller->rlcs |= RLCS_ERR | RLCS_NXM;
    }
}