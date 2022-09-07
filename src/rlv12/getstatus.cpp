#include "rlv12.h"

void RLV12::getStatus(RL01_2 &unit)
{
    // The Get Status bit (and ToDo: the Marker Bit) in the DAR must
    // be set.
    if (!(rlda & RLDA_GS))
    {
        // Operation incomplete; set error 
        setDone (RLCS_ERR | RLCS_INCMP);
        return;
    }

    // Reset errors
    // According to Table 4-6 in EK-RL012-UG-005 this also resets the
    // Volume Check condition
    if (rlda & RLDA_GS_CLR)
        unit.driveStatus_ &= ~(RLDS_ERR | RLDS_VCK);

    // Develop drive state
    rlmpr = (u16)(unit.driveStatus_ | (unit.currentDiskAddress_ & RLDS_HD));
    if (unit.rlStatus_ & RlStatus::UNIT_RL02)
        rlmpr |= RLDS_RL02;

    // Chect if unit is write-protected
    if (unit.rlStatus_ & RlStatus::UNIT_WLK || 
            unit.unitStatus_ & Status::UNIT_RO)
        rlmpr |= RLDS_WLK;

    if (unit.unitStatus_ & Status::UNIT_DIS || 
        unit.rlStatus_ & RlStatus::UNIT_OFFL)
    {
        rlmpr |= RLDS_DSE;
        setDone (RLCS_DRE | RLCS_INCMP);
    }

    rlmpr2 = rlmpr1 = rlmpr;

    // if (DEBUG_PRS(rl_dev))
    //    fprintf(sim_deb, ">>RL GSTA: rlds=%06o drv=%ld\n",
    //        rlmp, (long)(uptr - rl_dev.units));
    setDone (0);
}