#include "rl012.h"
#include "../rlv12.h"

// Constructor
// By default the unit is off-line. It is set on-line when a file is
// attached.
RL01_2::RL01_2 (PDP11Peripheral *owningDevice)
    :
    Unit (owningDevice),
    currentDiskAddress_ {0},
    rlStatus_ {RlStatus::UNIT_OFFL},
    driveStatus_ {0},
    running_ {true},
    seekTime_ {}
{
    seekTimerThread_ = std::thread(&RL01_2::seekTimer, this);
}

// Finish the seek timer thread
RL01_2::~RL01_2 ()
{
    // Wakeup the seek timer thread with the indication to finish
    running_ = false;
    startSeek_.notify_one ();

    // Wait till the thread has finished
    seekTimerThread_.join ();
}

