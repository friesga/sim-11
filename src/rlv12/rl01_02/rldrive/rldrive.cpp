#include "rldrive.h"
#include "rlv12/rlv12const.h"

RlDrive::RlDrive ()
    :
    driveStatus_ {0}
{
    driveThread_ = std::thread (&RlDrive::driveThread, this);
}

RlDrive::~RlDrive ()
{
    // Wakeup the drive thread with the indication to finish
    running_ = false;
    startSeek_.notify_one ();

    // Wait till the thread has finished
    driveThread_.join ();
}

// ToDo: seekTime to be passed in command
void RlDrive::startSeek (SimulatorClock::duration seekTime)
{
    seekTime_ = seekTime;
    startSeek_.notify_one ();
}

void RlDrive::waitForDriveReady ()
{
    std::lock_guard<std::mutex> guard {driveMutex_};
}

void RlDrive::driveThread ()
{
    // Guard against controller register access from the
    // command processor
    unique_lock<mutex> lock {driveMutex_};

    while (running_)
    {
        // Wait till we are signalled to start the timer and when
        // finished lock the heads on the cylinder.
        startSeek_.wait (lock);

        // After the specified seek time enter position mode with heads
        // locked on cylinder (i.e. seek completed).
        alarmClock_.sleepFor (seekTime_);
        driveStatus_ = (driveStatus_ & ~RLV12const::MPR_GS_State) |
            RLV12const::MPR_GS_LockOn;
    }
}
