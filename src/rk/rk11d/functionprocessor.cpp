#include "rk11d.h"
#include "overloaded.h"
#include "trace/trace.h"

#include <iostream>
#include <variant>
#include <functional>

using std::cerr;
using std::visit;
using std::get;
using std::bind;
using std::monostate;

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
    // The Event variant needs an explicicit initialization
    Event event {RKTypes::Function {}};

    while (functionQueue_.waitAndPop (event))
        functionProcessorStateMachine_->dispatch (event);
}
catch (const std::exception& ex)
{
    cerr << "RK11D::functionProcessor exception: " << ex.what () << '\n';
}

RK11D::StateMachine::StateMachine (RK11D* context)
    :
    context_ {context}
{}

// No actions to be executed at the entry of the WaitingForFunction state.
// The function is defined to satisfy the compiler.
void RK11D::StateMachine::entry (WaitingForFunction)
{}

// As its name suggests in the WaitingForFunction state, the function processor
// waits for a function to be processed. On the reception of a function a 
// transition is performed to either the ProcessingFunction or Polling state.
//
RK11D::State RK11D::StateMachine::transition (WaitingForFunction&&,
    RKTypes::Function function)
{
    if (function.rkcs.operation == RKTypes::Seek)
        return Polling {function};
    else
        return ProcessingFunction {function};
}

// In the WaitingForFunction states SeekCompleteReports are ignored
RK11D::State RK11D::StateMachine::transition (WaitingForFunction&& currentState,
    RKTypes::SeekCompleteReport)
{
    return move (currentState);
}

// At entry of the ProcessingFunction state the function defined in the
// transition to this state is processed. The state then waits for either
// another non-seek function (in which case the entry function is executed
// again) or a seek function. In that case a transition to the Polling state
// is taken. SeekCompleteEvents are ignored and the state machine transitions
// to the WaitingForFunction state.
//
void  RK11D::StateMachine::entry (ProcessingFunction current)
{
    context_->processFunction (current.function);
}

RK11D::State RK11D::StateMachine::transition (ProcessingFunction&&,
    RKTypes::Function function)
{
    if (function.rkcs.operation == RKTypes::Seek)
        return Polling {function};
    else
        return ProcessingFunction {function};
}

RK11D::State RK11D::StateMachine::transition (ProcessingFunction&&,
    RKTypes::SeekCompleteReport)
{
    return WaitingForFunction {};
}


// This state corresponds to the RK11-D's hardware polling functionality.
// In this state the function processor waits for seekCompleteReport's from
// the drives for which a seek is oustanding. In this state additional Seek
// functions can be issued. Other functions abort the hardware polling.
// 
// For every seek completion an interrupt is requested. When the interrupt is
// granted the next seek completion (if available) can be requested.
//
void RK11D::StateMachine::entry (Polling current)
{
    context_->processFunction (current.function);
}

// By returning a monostate object this transition is internal which means
// the state's entry action is not executed.
RK11D::State RK11D::StateMachine::transition (Polling&&,
    RKTypes::SeekCompleteReport report)
{
    if (context_->rkcs_.interruptOnDoneEnable)
    {
        // Wait for the previous interrupt request to be granted. As the
        // semaphore is initialized with the value one, the first aquire
        // will succeed. A subsequent aquire call will block until a release
        // of the semaphore (in completeSeek()) has been executed.
        context_->interruptRequestGranted_.acquire ();

        trace.debug ("Requesting interrupt for drive " + std::to_string (report.driveId));

        context_->bus_->requestInterrupt (TrapPriority::BR5, 5, 0, context_->vector_,
            bind (&RK11D::StateMachine::completeSeek, this, report));
    }
    else
        completeSeek (report);

    return monostate {};
}

RK11D::State RK11D::StateMachine::transition (Polling&&,
    RKTypes::Function function)
{
    if (function.rkcs.operation == RKTypes::Seek)
        return Polling {function};
    else
        return ProcessingFunction {function};
}


// This function is executed in the context of a transition of the
// StateMachine which means that the controller mutex
// is locked and register access is allowed.
void RK11D::StateMachine::completeSeek (RKTypes::SeekCompleteReport report)
{
    context_->selectedDrive_ = report.driveId;
    context_->rker_.value |= report.rker.value;

    trace.debug ("Interrupt handled for drive " + std::to_string (report.driveId));

    context_->interruptRequestGranted_.release ();
}

void RK11D::finish ()
{
    functionQueue_.close ();

    interruptRequestGranted_.release ();

    // Wake up the function processor
    functionAvailable_.notify_one ();
}