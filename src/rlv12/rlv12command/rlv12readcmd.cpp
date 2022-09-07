#include "rlv12readcmd.h"
#include "rlv12/rlv12.h"

void RLV12ReadCmd::execute (u16 *buffer, FILE *filePtr, u16 *rlcs, QBUS *bus)
{
    // Read Data or Read Data Without Header Check command
    // Read wordCount * 2 bytes; returned is the number of words read 
    // ToDo: Rename i
    int32_t i = fread (buffer, sizeof (int16_t), wordCount_, filePtr);

    // ToDo: Check error
    if (!ferror (filePtr))
    {
        // Clear the part of the buffer not filled by the read
        for (; i < wordCount_; i++)
            buffer[i] = 0;

        // Transfer words in buffer
        for (i = 0; i < wordCount_; memoryAddress_ += 2, ++i)
            if (!bus->writeWord (memoryAddress_, buffer[i]))
                *rlcs |= RLCS_ERR | RLCS_NXM;
    }
}