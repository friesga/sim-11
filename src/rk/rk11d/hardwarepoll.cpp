#include "rk11d.h"

#include <exception>
#include <iostream>

using std::cerr;

void RK11D::hardwarePoll ()
try
{
    // Guard against controller register access from writeWord() and the
    // function processor.
    unique_lock<mutex> lock {controllerMutex_};

    while (running_)
    {
        // The controllerMutex_ now is locked. Process events till the queue
        // is empty. The queue contains seek completions reported by a RK05
        // drive, start and stop poll events from the function processor and
        // bus priority level notifications.
        while (!pollEventQueue_.empty ())
        {
        }

        // Wait till we are signalled that a command completion has arrived
        // The wait() call unlocks the controllerMutex_. Note that the
        // seekComplete_ condition variable shares the mutex with the
        // actionAvailable_ condition variable. This ensures that the function
        // processor and the hardware poll won't be active at the same time.
        seekComplete_.wait (lock);
    }
}
catch (const std::exception& ex)
{
    cerr << "RK11D::hardwarePoll exception: " << ex.what () << '\n';
}