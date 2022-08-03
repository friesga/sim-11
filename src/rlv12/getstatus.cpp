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
        unit.status_ &= ~(RLDS_ERR | RLDS_VCK);

    // Develop drive state
    rlmpr = (u16)(unit.status_ | (unit.currentTrackHeadSector_ & RLDS_HD));
    if (unit.flags_ & UNIT_RL02)
        rlmpr |= RLDS_RL02;

    if (unit.flags_ & UNIT_WPRT)
        rlmpr |= RLDS_WLK;

    if (unit.flags_ & (UNIT_DIS | UNIT_OFFL))
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