#include "rlv12getstatuscmd.h"
#include "rlv12/rlv12.h"

#include <mutex>

using std::mutex;
using std::lock_guard;
 
//
// Perform a Get Status command
//
void RLV12GetStatusCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    // Guard against controller register access
	lock_guard<mutex> guard{ controller->controllerMutex_ };

    // The Get Status bit (and ToDo: the Marker Bit) in the DAR must
    // be set.
    if (!(controller->rlda & RLDA_GS))
    {
        // Operation incomplete; set error 
        controller->setDone (RLCS_ERR | RLCS_INCMP);
        return;
    }

    // Reset errors
    // According to Table 4-6 in EK-RL012-UG-005 this also resets the
    // Volume Check condition
    if (controller->rlda & RLDA_GS_CLR)
        unit->driveStatus_ &= ~(RLDS_ERR | RLDS_VCK);

    // Develop drive state
    // controller->rlmpr = 
    //     (u16)(unit->driveStatus_ | (unit->currentDiskAddress_ & RLDS_HD));
    if (unit->rlStatus_ & RlStatus::UNIT_RL02)
        controller->rlmpr |= RLDS_RL02;

    // Chect if unit is write-protected
    if (unit->rlStatus_ & RlStatus::UNIT_WLK || 
            unit->unitStatus_ & Status::UNIT_RO)
        controller->rlmpr |= RLDS_WLK;

    if (unit->unitStatus_ & Status::UNIT_DIS || 
        unit->rlStatus_ & RlStatus::UNIT_OFFL)
    {
        controller->rlmpr |= RLDS_DSE;
        controller->setDone (RLCS_DRE | RLCS_INCMP);
    }

    controller->rlmpr2 = controller->rlmpr1 = controller->rlmpr;

    controller->setDone (0);
}

// No action to finish this command
void RLV12GetStatusCmd::finish (RLV12 *controller, RL01_2 *unit)
{}