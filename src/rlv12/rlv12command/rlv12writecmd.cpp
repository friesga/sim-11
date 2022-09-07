#include "rlv12writecmd.h"
#include "rlv12/rlv12.h"

void RLV12WriteCmd::execute (u16* buffer, FILE* filePtr,
    u16* rlcs, QBUS* bus)
{
    CondData<u16> tmpValue;
    // ToDo: Rename i
    int32_t i;

    for (i = 0; i < wordCount_; memoryAddress_ += 2, ++i)
    {
        tmpValue = bus->read (memoryAddress_).valueOr (0);
        if (!tmpValue.hasValue ())
        {
            *rlcs |= RLCS_ERR | RLCS_NXM;
            // Set adj xfer length
            wordCount_ -= i;
            break;
        }
        buffer[i] = tmpValue;
    }

    // Any xfer?
    if (wordCount_)
    {
        // Clear to end of block
        // ToDo: Rename awc
        int32_t awc = (wordCount_ + (RL_NUMWD - 1)) & ~(RL_NUMWD - 1);
        for (i = wordCount_; i < awc; i++)
            buffer[i] = 0;

        fwrite (buffer, sizeof (int16_t), awc, filePtr);
        // ToDo: Handle possible fwrite error
        // err = ferror (filePtr_);
    }
}