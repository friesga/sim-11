#include "rl01_02.h"
#include "rlv12/rlv12.h"

using std::mutex;
using std::unique_lock;

// Simulate the execution (of the head movement) of a Seek command
void RL01_02::seekTimer ()
{
    // Guard against controller register access from the
    // command processor
    unique_lock<mutex> lock {driveMutex_};

    while (running_)
    {
        // Wait till we are signalled to start the timer and when
        // finished lock the heads on the cylinder.
        startSeek_.wait (lock);

        alarmClock_.sleepFor (seekTime_);
        driveStatus_ =
            (driveStatus_ & ~RLV12const::MPR_GS_State) | RLV12const::MPR_GS_LockOn;
    }
}