#include "rl01_02.h"

// The following function is executed in a seperate thread.
void RL01_02::driveThread ()
{
    // Guard against controller register access from the
    // command processor
    unique_lock<mutex> lock {driveMutex_};

    while (running_)
    {
        // The driveMutex_ now is locked. Process events till the queue
        // is empty.
        while (!eventQueue_.empty ())
        {
            Event event = eventQueue_.front ();
            stateMachine_->dispatch (event);

            // The event has been processed
            eventQueue_.pop ();
        }

        // Wait till we are signalled that a command is ready to be processed
        // 
        // wait() unlocks the DriveMutex_.
        startCommand_.wait (lock);
    }
}