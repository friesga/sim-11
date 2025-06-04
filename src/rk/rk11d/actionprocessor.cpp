#include "rk11d.h"

#include "overloaded.h"

#include <iostream>
#include <variant>

using std::cerr;
using std::visit;

// The RK11-D functionality is partly synchronous and partly asynchronous
// in nature; data transfer functions are handled synchronously and Seek and
// Control Reset functions are processed asynchronously.
// 
// The action processor processes both newly started functions (started by
// the program running on the CPU) and seek completions. When a Seek function
// is initiated, the action processor becomes active and continues polling for
// seek completions. However, if a transfer function follows, the action
// processor is stopped and the transfer command is processed, up to and
// including a command complete return notification. After that, the action
// processor continues again.
// 
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
        // is empty. The queue contains either functions issued by the running
        // program or drive conditions reported by a RK05 drive.
        //
        while (!actionQueue_.empty ())
        {
            // Use an overloaded visitor to process either a function or a
            // drive condition.
            visit (overloaded
                {
                    [this] (RKTypes::Function function)
                        { processFunction (function); },
                    [this] (RKTypes::DriveCondition driveCondition)
                        { processDriveCondition (driveCondition); }
                },
                actionQueue_.front ());

            // The event has been processed
            actionQueue_.pop ();
        }

        // Wait till we are signalled that a command is ready to be processed
        // 
        // wait() unlocks the controllerMutex_.
        actionAvailable_.wait (lock);
    }
}
catch (const std::exception& ex)
{
    cerr << "RK11D::actionProcessor exception: " << ex.what () << '\n';
}

void RK11D::finish ()
{
    // Guard against controller register access from main thread
    std::lock_guard<std::mutex> guard {controllerMutex_};

    running_ = false;

    // Wake up the action processor
    actionAvailable_.notify_one ();
}