#include "rk11d.h"

#include <iostream>

using std::cerr;

// The action processor is executed in a seperate thread.
//
void RK11D::actionProcessor ()
try
{
    // Guard against controller register access from writeWord()
    unique_lock<mutex> lock {controllerMutex_};

    while (running_)
    {
        // The controllerMutex_ now is locked. Process events till the queue
        // is empty.
        while (!rk11ActionQueue_.empty ())
        {
            RKDefinitions::Function action = rk11ActionQueue_.front ();

            // ToDo: Process action


            // The event has been processed
            rk11ActionQueue_.pop ();
        }

        // Wait till we are signalled that a command is ready to be processed
        // 
        // wait() unlocks the DriveMutex_.
        actionAvailable_.wait (lock);
    }
}
catch (const std::exception& ex)
{
    cerr << "RK11D::actionProcessor exception: " << ex.what () << '\n';
}