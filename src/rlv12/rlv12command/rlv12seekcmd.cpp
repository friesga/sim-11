#include "rlv12seekcmd.h"
#include "rlv12/rlv12.h"

void RLV12SeekCmd::execute (RLV12 *controller, RL01_2 *unit)
{
    int32_t currentCylinder;
    int32_t offset;
    int32_t newCylinder;
    int32_t maxCylinder;
    int32_t seekTime;

    //
    // EK-ORL11-TD-001, p2-3: "If the CPU software initiates another
    // operation on a drive that is busy seeking, the controller will
    // suspend the operation until the seek is completed."
    // 
    if (unit->unitStatus_ & Status::UNIT_DIS || 
        unit->rlStatus_ & RlStatus::UNIT_OFFL || 
        !(unit->unitStatus_ & Status::UNIT_ATT))
    {
        // No cartridge available to perform a seek on
        controller->setDone (RLCS_ERR | RLCS_INCMP);
        unit->driveStatus_ |= RLDS_STO;
        return;
    }

    currentCylinder = getCylinder (unit->currentDiskAddress_);

    // ToDo: offset to be passed in command
    offset = getCylinder (controller->rlda);

    // Seek direction in or out?
    // According to the RL01/RL02 User Guide (EK-RL012-UG-005), par 4.3.4: 
    // If the difference word is large enough that the heads attempt to move
    // past the innermost or outermost limits, the head will stop at the
    // guard band and retreat to the first even-numbered data track.
    if (controller->rlda & RLDA_SK_DIR)
    {
        // Outwards
        newCylinder = currentCylinder + offset;
        maxCylinder = (unit->rlStatus_ & RlStatus::UNIT_RL02) ? 
            RL_NUMCY * 2 : RL_NUMCY;
        if (newCylinder >= maxCylinder)
            newCylinder = maxCylinder - 2;
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
        unit->driveStatus_ = (unit->driveStatus_ & ~RLDS_M_STATE) | RLDS_SEEK;

    // ToDo: If a head switch, sector should be RL_NUMSC/2?
    // Put on track
    unit->currentDiskAddress_ = (newCylinder << RLDA_V_CYL) |
        ((controller->rlda & RLDA_SK_HD) ? RLDA_HD1 : RLDA_HD0);

    // Real timing (EK-RLV12-TD-001 and EK-RL012-UG-005):
    // minimum 6.5ms, maximum 15ms for head switch,
    // maximum 17ms for 1 track seek w/head switch, 
    // 55ms average seek, 100ms maximum seek
    // 
    // Try to simulate this timing by the following formula
    seekTime = 17 + (0.4 * abs (newCylinder - currentCylinder));
    
    controller->setDone (0);
    std::this_thread::sleep_for (std::chrono::milliseconds (seekTime));  

    // After wait interval enter position mode with heads locked on
    // cylinder (i.e. seek completed)
    unit->driveStatus_ = (unit->driveStatus_ & ~RLDS_M_STATE) | RLDS_LOCK; 
    return;
}

// No action to finish this command
void RLV12SeekCmd::finish (RLV12 *controller, RL01_2 *unit)
{
}