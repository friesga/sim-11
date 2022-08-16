#include "rlv12.h"
#include "asynctimer/asynctimer.h"

#include <chrono>

// Seek function
void RLV12::seek (RL01_2& unit)
{
    int32_t currentCylinder;
    int32_t offset;
    int32_t newCylinder;
    int32_t maxCylinder;
    int32_t tim;

    // Note the function accepts Seek Commands for a drive while another
    // seek is in progress and the Drive Ready bit isn't set. In this case
    // the command is accepted and is executed after completion of the 
    // previous seek command. (EK-ORL11-TD-001, p2-3: "If the CPU software
    // initiates another operation on a drive that is busy seeking, the
    // controller will suspend the operation until the seek is completed."
    // 
    // As in the service route for a seek just the Locked On bit is set,
    // a following seek can be executed immediately.
    //
    if (unit.unitStatus_ & Status::UNIT_DIS || 
        unit.rlStatus_ & RlStatus::UNIT_OFFL || 
        !(unit.unitStatus_ & Status::UNIT_ATT))
    {
        // No cartridge available to perform a seek on
        setDone (RLCS_ERR | RLCS_INCMP);
        unit.driveStatus_ |= RLDS_STO;
        return;
    }

    currentCylinder = getCylinder (unit.currentTrackHeadSector_);
    offset = getCylinder (rlda);

    // Seek direction in or out?
    if (rlda & RLDA_SK_DIR)
    {
        // Outwards
        newCylinder = currentCylinder + offset;
        maxCylinder = (unit.rlStatus_ & RlStatus::UNIT_RL02) ? 
            RL_NUMCY * 2 : RL_NUMCY;
        if (newCylinder >= maxCylinder)
            newCylinder = maxCylinder - 1;
    }
    else
    {
        // Inwards
        newCylinder = currentCylinder - offset;
        if (newCylinder < 0)
            newCylinder = 0;
    }

    // Enter velocity mode? Only if a different cylinder
    if (newCylinder != currentCylinder)
        // Move the positioner 
        unit.driveStatus_ = (unit.driveStatus_ & ~RLDS_M_STATE) | RLDS_SEEK;

    // ToDo: If a head switch, sector should be RL_NUMSC/2?
    // Put on track
    unit.currentTrackHeadSector_ = (newCylinder << RLDA_V_CYL) |
        ((rlda & RLDA_SK_HD) ? RLDA_HD1 : RLDA_HD0);

    // Real timing:
    // min 6.5ms, max 15ms for head switch,
    // max 17ms for 1 track seek w/head switch
    // 55ms avg seek
    // 100ms max seek

    tim = abs (newCylinder - currentCylinder);
    if (tim == 0)
        tim++;

    tim *= rl_swait;
    // if (DEBUG_PRS(rl_dev))
    //     fprintf(sim_deb, ">>RL SEEK: drv %d, dist %d, head sw %d, tim %d\n",
    //        (int32)(uptr - rl_dev.units),
    //         abs(newCylinder - currentCylinder), (rlda & RLDA_SK_HD), tim);
    // ToDo: Change RLCS_ macro's to enum
    unit.function_ = RLCS_SEEK;
    // sim_activate(uptr, tim);               /* must be > 0 */
    timer.start (&unit, std::chrono::milliseconds (tim));
    setDone (0);
}