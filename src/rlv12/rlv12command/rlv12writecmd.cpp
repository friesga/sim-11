#include "rlv12writecmd.h"
#include "rlv12/rlv12.h"

void RLV12WriteCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    CondData<u16> tmpValue;
    // ToDo: Rename i
    int32_t i;

    if (unit->unitStatus_ & Status::UNIT_RO || 
        unit->rlStatus_ & RlStatus::UNIT_WLK)
    {
        // Write and locked
        unit->driveStatus_ |= RLDS_WGE;                     
        controller->setDone (RLCS_ERR | RLCS_DRE);
        return;
    }

    for (i = 0; i < wordCount_; memoryAddress_ += 2, ++i)
    {
        tmpValue = controller->bus->read (memoryAddress_).valueOr (0);
        if (!tmpValue.hasValue ())
        {
            controller->rlcs |= RLCS_ERR | RLCS_NXM;
            // Set adj xfer length
            wordCount_ -= i;
            break;
        }
        controller->rlxb_[i] = tmpValue;
    }

    // Any xfer?
    if (wordCount_)
    {
        // Clear to end of block
        // ToDo: Rename awc
        int32_t awc = (wordCount_ + (RL_NUMWD - 1)) & ~(RL_NUMWD - 1);
        for (i = wordCount_; i < awc; i++)
            controller->rlxb_[i] = 0;

        fwrite (controller->rlxb_, sizeof (int16_t), awc, unit->filePtr_);
        // ToDo: Handle possible fwrite error
        // err = ferror (filePtr_);
    }
}