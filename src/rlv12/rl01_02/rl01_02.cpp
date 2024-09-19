#include "rl01_02.h"
#include "../rlv12.h"

// Constructor
// By default the unit is off-line. It is set on-line when a file is
// attached.
RL01_02::RL01_02 (PDP11Peripheral *owningDevice)
    :
    Unit (owningDevice),
    currentDiskAddress_ {0},
    rlStatus_ {RlStatus::UNIT_OFFL},
    driveStatus_ {0},
    running_ {true},
    seekTime_ {}
{
    seekTimerThread_ = std::thread(&RL01_02::seekTimer, this);
}

// Finish the seek timer thread
RL01_02::~RL01_02 ()
{
    // Wakeup the seek timer thread with the indication to finish
    running_ = false;
    startSeek_.notify_one ();

    // Wait till the thread has finished
    seekTimerThread_.join ();
}

// Calculate the position of a sector as an offset in the file from
// the specified diskAddress
int32_t RL01_02::filePosition (int32_t diskAddress) const
{
    return (RLV12const::getTrack (diskAddress) * RLV12const::sectorsPerSurface +
        RLV12const::getSector (diskAddress)) * RLV12const::wordsPerSector *
        sizeof (int16_t);
}
