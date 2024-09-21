#include "rldrive.h"
#include "rlv12/rlv12const.h"

#include <cassert>

using std::holds_alternative;

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
    startCommand_.notify_one ();

    // Wait till the thread has finished
    driveThread_.join ();
}

// Push the given command in the queue and signal the drive thread a command
// is waiting to be processed.
void RlDrive::execute (DriveCommand command)
{
    commandQueue_.push (command);
    startCommand_.notify_one ();
}

void RlDrive::waitForDriveReady ()
{
    std::lock_guard<std::mutex> guard {driveMutex_};
}

// The following function is executed in a seperate thread.
void RlDrive::driveThread ()
{
    // Guard against controller register access from the
    // command processor
    unique_lock<mutex> lock {driveMutex_};

    while (running_)
    {
        // Wait till we are signalled that a command is ready to be processed
        // 
        // wait() unlocks the DriveMutex_.
        startCommand_.wait (lock);

        // The driveMutex_ now is locked. Process commands till the queue
        // is empty.
        while (!commandQueue_.empty ())
        {
            DriveCommand driveCommand = commandQueue_.front ();

            // For now the only supported command is a seek.
            assert (holds_alternative<SeekCommand> (driveCommand));
                        
            // After the specified seek time enter position mode with heads
            // locked on cylinder (i.e. seek completed).
            alarmClock_.sleepFor (get<SeekCommand> (driveCommand).seekTime);
            driveStatus_ = (driveStatus_ & ~RLV12const::MPR_GS_State) |
                RLV12const::MPR_GS_LockOn;

            // The command has been processed
            commandQueue_.pop ();
        }
    }
}
