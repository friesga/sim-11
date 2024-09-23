#include "rldrive.h"
#include "rlv12/rlv12const.h"

#include <cassert>

using std::holds_alternative;
using std::make_unique;

RlDrive::RlDrive ()
    :
    driveStatus_ {0}
{
    driveThread_ = std::thread (&RlDrive::driveThread, this);
    stateMachine_ = make_unique<StateMachine> (this);
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
void RlDrive::startSeek (SimulatorClock::duration seekTime)
{
    eventQueue_.push (SeekCommand {seekTime});
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

        // The driveMutex_ now is locked. Process events till the queue
        // is empty.
        while (!eventQueue_.empty ())
        {
            Event event = eventQueue_.front ();
            stateMachine_->dispatch (event);

            // The event has been processed
            eventQueue_.pop ();
        }
    }
}
