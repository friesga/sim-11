#include "cmdprocessor.h"

//
// Perform a Get Status command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::getStatusCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    // The Get Status bit (and ToDo: the Marker Bit) in the DAR must
    // be set.
    if (!(controller_->rlda & RLDA_GS))
    {
        // Operation incomplete; set error 
        return RLCS_ERR | RLCS_INCMP;
    }

    // Reset errors
    // According to Table 4-6 in EK-RL012-UG-005 this also resets the
    // Volume Check condition
    if (controller_->rlda & RLDA_GS_CLR)
        unit->driveStatus_ &= ~(RLDS_ERR | RLDS_VCK);

    // Develop drive state
    // controller->rlmpr = 
    //     (u16)(unit->driveStatus_ | (unit->currentDiskAddress_ & RLDS_HD));
    if (unit->rlStatus_ & RlStatus::UNIT_RL02)
        controller_->rlmpr |= RLDS_RL02;

    // Chect if unit is write-protected
    if (unit->rlStatus_ & RlStatus::UNIT_WLK || 
            unit->unitStatus_ & Status::UNIT_RO)
        controller_->rlmpr |= RLDS_WLK;

    if (unit->unitStatus_ & Status::UNIT_DIS || 
        unit->rlStatus_ & RlStatus::UNIT_OFFL)
    {
        controller_->rlmpr |= RLDS_DSE;
        return RLCS_DRE | RLCS_INCMP;
    }

    controller_->rlmpr2 = controller_->rlmpr1 = controller_->rlmpr;

    return 0;
}