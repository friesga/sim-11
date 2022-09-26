#include "rlv12writecmd.h"
#include "rlv12/rlv12.h"

#include <mutex>

using std::mutex;
using std::lock_guard;

void RLV12WriteCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    CondData<u16> tmpValue;

    // Set position in file to the block to be written
    if (fseek (unit->filePtr_, filePosition(), SEEK_SET))
        return;

    if (unit->unitStatus_ & Status::UNIT_RO || 
        unit->rlStatus_ & RlStatus::UNIT_WLK)
    {
        // Write and locked
        lock_guard<mutex> guard{ controller->controllerMutex_ };
        unit->driveStatus_ |= RLDS_WGE;                     
        controller->setDone (RLCS_ERR | RLCS_DRE);
        return;
    }

    for (int32_t index = 0, memAddr = memoryAddress_;
        index < wordCount_; memAddr += 2, ++index)
    {
        tmpValue = controller->bus->read (memAddr).valueOr (0);
        if (!tmpValue.hasValue ())
        {
            lock_guard<mutex> guard{ controller->controllerMutex_ };
            controller->rlcs |= RLCS_ERR | RLCS_NXM;
            // Set adj xfer length
            wordCount_ -= index;
            break;
        }
        controller->rlxb_[index] = tmpValue;
    }

    // Any xfer?
    if (wordCount_)
    {
        // Clear to end of block
        // ToDo: Rename awc
        size_t awc = (wordCount_ + (RL_NUMWD - 1)) & ~(RL_NUMWD - 1);
        for (size_t index = wordCount_; index < awc; ++index)
            controller->rlxb_[index] = 0;

        fwrite (controller->rlxb_, sizeof (int16_t), awc, unit->filePtr_);
        // ToDo: Handle possible fwrite error
        // err = ferror (filePtr_);
    }
}