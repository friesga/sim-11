#include "rlv12writecheckcmd.h"
#include "rlv12/rlv12.h"


void RLV12WriteCheckCmd::execute (RLV12 *controller, RL01_2 *unit)
{
        // Write Check Command
    CondData<u16> comp;

    // ToDo: Rename i
    int32_t i = fread (controller->rlxb_, sizeof (int16_t), wordCount_, unit->filePtr_);
    if (!ferror (unit->filePtr_))
    {
        // Clear remainder of buffer
        for (; i < wordCount_; i++)
            controller->rlxb_[i] = 0;

        // Save wordCount
        // ToDo: Rename awc
        int32_t awc = wordCount_;
        for (wordCount_ = 0; wordCount_ < awc; 
            memoryAddress_ += 2, ++wordCount_)
        {
            // Loop through buffer
            comp = controller->bus->read (memoryAddress_).valueOr (0);
            if (!comp.hasValue ())
            {
                controller->rlcs |= RLCS_ERR | RLCS_NXM;
                break;
            }

            // Check read word with buffer
            // ToDo: Quit for loop when an inequality is detected?
            if (comp != controller->rlxb_[wordCount_])
                controller->rlcs = RLCS_ERR | RLCS_CRC;
        }
    }
}