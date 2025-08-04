#include "rk11d.h"
#include "overloaded.h"
#include "trace/trace.h"

#include <iostream>
#include <variant>

using std::cerr;
using std::visit;
using std::get;

// The RK11-D functionality is partly synchronous and partly asynchronous
// in nature; data transfer functions are handled synchronously and Seek and
// Control Reset functions are processed asynchronously.
// 
// The function processor processes newly initiated functions (started by
// the program running on the CPU).
// 
// The function processor is executed in a seperate thread.
//
void RK11D::functionProcessor ()
try
{
    // Guard against controller register access from writeWord()
    unique_lock<mutex> lock {controllerMutex_};

    while (running_)
    {
        // The controllerMutex_ now is locked. Process events till the queue
        // is empty.
        //
        while (!functionQueue_.empty ())
        {
            trace.rk11Function (get<RKTypes::Function> (functionQueue_.front ()));
            functionProcessorStateMachine_->dispatch (functionQueue_.front ());
            functionQueue_.pop ();
            setControlReady ();
        }

        // Wait till we are signalled that a function is ready to be processed
        // 
        // wait() unlocks the controllerMutex_.
        functionAvailable_.wait (lock);
    }
}
catch (const std::exception& ex)
{
    cerr << "RK11D::functionProcessor exception: " << ex.what () << '\n';
}

RK11D::FunctionProcessorStateMachine::FunctionProcessorStateMachine (RK11D* context)
    :
    context_ {context}
{}


RK11D::FunctionProcessorState RK11D::FunctionProcessorStateMachine::transition (WaitingForFunction&& currentState,
    FunctionProcessorEvent event)
{
    context_->processFunction (get <RKTypes::Function> (event));
    return move (currentState);
}


void RK11D::finish ()
{
    // Guard against controller register access from main thread
    std::lock_guard<std::mutex> guard {controllerMutex_};

    running_ = false;

    // Wake up the function processor
    functionAvailable_.notify_one ();

    // Finish the harware poll function
    pollEventQueue_.close ();
    interruptRequestGranted_.release ();
}