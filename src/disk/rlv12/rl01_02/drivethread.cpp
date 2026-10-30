#include "rl01_02.h"

#include <iostream>

using std::cerr;

// The following function is executed in a seperate thread.
void RL01_02::driveThread ()
try
{
    // Guard against simultaneous access of the eventQueue_.
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
catch (const std::exception& ex)
{
    cerr << "RL01_02::driveThread exception: " << ex.what () << '\n';
}