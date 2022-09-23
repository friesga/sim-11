#include "rlv12writecheckcmd.h"
#include "rlv12/rlv12.h"

// Write Check Command
void RLV12WriteCheckCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    CondData<u16> comp;

    // Set position in file to the block to be read
    if (fseek (unit->filePtr_, filePosition(), SEEK_SET))
        return;

    size_t numBytes = fread (controller->rlxb_, sizeof (int16_t), 
        wordCount_, unit->filePtr_);

    if (!ferror (unit->filePtr_))
    {
        // Clear remainder of buffer
        for (size_t index = numBytes; index < wordCount_; ++index)
            controller->rlxb_[index] = 0;

        // Save wordCount
        // ToDo: Rename awc
        int32_t awc = wordCount_;
        int32_t memAddr = memoryAddress_;
        for (wordCount_ = 0; wordCount_ < awc; 
                memAddr += 2, ++wordCount_)
        {
            // Loop through buffer
            comp = controller->bus->read (memAddr).valueOr (0);
            if (!comp.hasValue ())
            {
                controller->rlcs |= RLCS_ERR | RLCS_NXM;
                break;
            }

            // Check read word with buffer
            // ToDo: Quit for loop when an inequality is detected?
            if (comp != controller->rlxb_[wordCount_])
                controller->rlcs |= RLCS_ERR | RLCS_CRC;
        }
    }
}