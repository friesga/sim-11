#include "rlv12writecheckcmd.h"
#include "rlv12/rlv12.h"

void RLV12WriteCheckCmd::execute (u16* buffer, FILE* filePtr,
    u16* rlcs, QBUS* bus)
{
    // Write Check Command
    CondData<u16> comp;

    // ToDo: Rename i
    int32_t i = fread (buffer, sizeof (int16_t), wordCount_, filePtr);
    if (!ferror (filePtr))
    {
        // Clear remainder of buffer
        for (; i < wordCount_; i++)
            buffer[i] = 0;

        // Save wordCount
        // ToDo: Rename awc
        int32_t awc = wordCount_;
        for (wordCount_ = 0; wordCount_ < awc; 
            memoryAddress_ += 2, ++wordCount_)
        {
            // Loop through buffer
            comp = bus->read (memoryAddress_).valueOr (0);
            if (!comp.hasValue ())
            {
                *rlcs |= RLCS_ERR | RLCS_NXM;
                break;
            }

            // Check read word with buffer
            // ToDo: Quit for loop when an inequality is detected?
            if (comp != buffer[wordCount_])
                *rlcs = RLCS_ERR | RLCS_CRC;
        }
    }
}