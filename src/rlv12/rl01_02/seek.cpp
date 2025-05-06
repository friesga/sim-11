#include "rl01_02.h"
#include "rlv12/rlv12.h"

// The disk address register contains three parameters for the seek operation:
// - Cylinder address difference (bits 7-15),
// - Direction of head movement (bit 2),
// - Head Select (bit 4).
//
// The head movement is indepent of the Head Select parameter but partly
// determines the current sector address.
//
void RL01_02::seek (u16 diskAddressRegister)
{
    int32_t currentCylinder;
    int32_t offset;
    int32_t newCylinder;
    int32_t maxCylinder;

    //
    // EK-ORL11-TD-001, p2-3: "If the CPU software initiates another
    // operation on a drive that is busy seeking, the controller will
    // suspend the operation until the seek is completed."
    waitForSeekComplete ();

    currentCylinder = RLV12const::getCylinder (currentDiskAddress_);

    offset = RLV12const::getCylinder (diskAddressRegister);

    // Seek direction in or out?
    // According to the RL01/RL02 User Guide (EK-RL012-UG-005), par 4.3.4: 
    // If the difference word is large enough that the heads attempt to move
    // past the innermost or outermost limits, the head will stop at the
    // guard band and retreat to the first even-numbered data track.
    if (diskAddressRegister & RLV12const::DAR_Direction)
    {
        // Outwards
        newCylinder = currentCylinder + offset;
        maxCylinder = (rlStatus_ & RlStatus::UNIT_RL02) ?
            RLV12const::RL01cylindersPerCartridge * 2 : RLV12const::RL01cylindersPerCartridge;
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
    //    unit->driveStatus_ = (unit->driveStatus_ & ~MPR_GS_State) | RLDS_SEEK;

    // ToDo: If a head switch, sector should be sectorsPerSurface/2?
    // Put on track
    currentDiskAddress_ = (newCylinder << RLV12const::DAR_CylinderPosition) |
        ((diskAddressRegister & RLV12const::DAR_HeadSelect) ?
            RLV12const::DAR_Head1 : RLV12const::DAR_Head0);

    // Real timing (EK-RLV12-TD-001 and EK-RL012-UG-005):
    // minimum 6.5ms, maximum 15ms for head switch,
    // maximum 17ms for 1 track seek w/head switch, 
    // 55ms average seek, 100ms maximum seek
    // 
    // Try to simulate this timing by the following formula
    seekTime_ = std::chrono::milliseconds (static_cast<uint64_t>
        (6.5 + (0.04 * abs (newCylinder - currentCylinder))));

    // Start a drive seek. Push a seek command in the drive event queue and
    // signal the drive thread an event is waiting to be processed.
    unique_lock<mutex> lock {driveMutex_};
    eventQueue_.push (SeekCommand {seekTime_});
    startCommand_.notify_one ();
}