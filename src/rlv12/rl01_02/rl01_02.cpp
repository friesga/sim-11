#include "rl01_02.h"
#include "../rlv12.h"

using std::make_unique;

// Constructor
// By default the unit is off-line. It is set on-line when a file is
// attached.
RL01_02::RL01_02 (PDP11Peripheral *owningDevice)
    :
    Unit (owningDevice),
    currentDiskAddress_ {0},
    rlStatus_ {RlStatus::UNIT_OFFL},
    seekTime_ {}
{
    driveThread_ = std::thread (&RL01_02::driveThread, this);
    stateMachine_ = make_unique<StateMachine> (this);
}

// Finish the drive thread
RL01_02::~RL01_02 ()
{
    // Wakeup the drive thread with the indication to finish
    running_ = false;
    startCommand_.notify_one ();

    // Wait till the thread has finished
    driveThread_.join ();
}

// Calculate the position of a sector as an offset in the file from
// the specified diskAddress
int32_t RL01_02::filePosition (int32_t diskAddress) const
{
    return (RLV12const::getTrack (diskAddress) * RLV12const::sectorsPerSurface +
        RLV12const::getSector (diskAddress)) * RLV12const::wordsPerSector *
        sizeof (int16_t);
}

void RL01_02::waitForDriveReady ()
{
    std::lock_guard<std::mutex> guard {driveMutex_};
}