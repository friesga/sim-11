#include "cmdprocessor.h"

#include <chrono>
#include <future>

//
// Perform a Seek command for the specified unit with the 
// specified parameters.
//
u16 CmdProcessor::seekCmd (RL01_2 *unit, RLV12Command &rlv12Command)
{
    int32_t currentCylinder;
    int32_t offset;
    int32_t newCylinder;
    int32_t maxCylinder;

    // Verify the unit is available
    if (!unitAvailable (unit))
    {
        // Set seek time-out. Note that this status differs from
        // the status returned by data transfer commands if the
        // unit is unavailable.
        unit->driveStatus_ |= RLDS_STO;

        // Flag error
        return RLCS_ERR | RLCS_INCMP;
    }

    //
    // EK-ORL11-TD-001, p2-3: "If the CPU software initiates another
    // operation on a drive that is busy seeking, the controller will
    // suspend the operation until the seek is completed."
    // 
    // Guard against drive access while the seek is running
	std::unique_lock<std::mutex> lock {unit->driveMutex_};

    currentCylinder = getCylinder (unit->currentDiskAddress_);

    // ToDo: offset to be passed in command
    offset = getCylinder (controller_->rlda);

    // Seek direction in or out?
    // According to the RL01/RL02 User Guide (EK-RL012-UG-005), par 4.3.4: 
    // If the difference word is large enough that the heads attempt to move
    // past the innermost or outermost limits, the head will stop at the
    // guard band and retreat to the first even-numbered data track.
    if (controller_->rlda & RLDA_SK_DIR)
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
    // The Seek status has already been set in writeWord() as it cannot
    // be guaranteed the status will be set at this point before the
    // host software reads the CSR
    // if (newCylinder != currentCylinder)
    //    // Move the positioner 
    //    unit->driveStatus_ = (unit->driveStatus_ & ~RLDS_M_STATE) | RLDS_SEEK;

    // ToDo: If a head switch, sector should be RL_NUMSC/2?
    // Put on track
    unit->currentDiskAddress_ = (newCylinder << RLDA_V_CYL) |
        ((controller_->rlda & RLDA_SK_HD) ? RLDA_HD1 : RLDA_HD0);

    // Real timing (EK-RLV12-TD-001 and EK-RL012-UG-005):
    // minimum 6.5ms, maximum 15ms for head switch,
    // maximum 17ms for 1 track seek w/head switch, 
    // 55ms average seek, 100ms maximum seek
    // 
    // Try to simulate this timing by the following formula
    unit->seekTime_ = 17 + (0.4 * abs (newCylinder - currentCylinder));

    // Wakeup the seekTimer thread for the unit. After the specified seek
    // time enter position mode with heads locked on cylinder (i.e. seek
    // completed).
    lock.unlock ();
    unit->startSeek_.notify_one ();
        
    return 0;
}