#include "rl012.h"

// Simulate the execution (of the head movement) of a Seek command
void RL01_2::seekTimer ()
{
    // Guard against controller register access from the
    // command processor
    std::unique_lock<std::mutex> lock{ driveMutex_ };

    while (running_)
    {
        // Wait till we are signalled to start the timer and when
        // finished lock the heads on the cylinder.
        startSeek_.wait (lock);

        std::this_thread::sleep_for (std::chrono::milliseconds (seekTime_));
        driveStatus_ =
            (driveStatus_ & ~RLDS_M_STATE) | RLDS_LOCK;
    }
}