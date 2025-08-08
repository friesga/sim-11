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
    // The FunctionProcessorEvent variant needs an explicicit initialization
    FunctionProcessorEvent event {RKTypes::Function {}};

    while (functionQueue_.waitAndPop (event))
        functionProcessorStateMachine_->dispatch (event);
}
catch (const std::exception& ex)
{
    cerr << "RK11D::functionProcessor exception: " << ex.what () << '\n';
}

RK11D::FunctionProcessorStateMachine::FunctionProcessorStateMachine (RK11D* context)
    :
    context_ {context}
{}

// As its name suggests in the WaitingForFunction state, the function processor
// waits for a function to be processed. On the reception of a function, that
// function is executed, followed by a transition to either the current state,
// or - in case of a Seek function - to the Polling state.
//
RK11D::FunctionProcessorState RK11D::FunctionProcessorStateMachine::transition (WaitingForFunction&& currentState,
    RKTypes::Function function)
{
    context_->processFunction (function);
    
    if (function.rkcs.operation == RKTypes::Seek)
        return Polling {};

    return move (currentState);
}

// In the WaitingForFunction states SeekCompleteReports are ignored
RK11D::FunctionProcessorState RK11D::FunctionProcessorStateMachine::transition (WaitingForFunction&& currentState,
    RKTypes::SeekCompleteReport)
{
    return move (currentState);
}

// This state corresponds to the RK11-D's hardware polling functionality.
// In this state the function processor waits for seekCompleteReport's from
// the drives for which a seek is oustanding. In this state additional Seek
// functions can be issued. Other functions abort the hardware polling.
// 
// For every seek completion an interrupt is requested. When the interrupt is
// granted the next seek completion (if available) can be requested.
//
RK11D::FunctionProcessorState RK11D::FunctionProcessorStateMachine::transition (Polling&& currentState,
    RKTypes::SeekCompleteReport report)
{
    if (context_->rkcs_.interruptOnDoneEnable)
    {
        context_->interruptRequestGranted_.acquire ();

        trace.debug ("Requesting interrupt for drive " + std::to_string (report.driveId));

        context_->bus_->requestInterrupt (TrapPriority::BR5, 5, 0, context_->vector_,
            bind (&RK11D::FunctionProcessorStateMachine::completeSeek, this, report));
    }
    else
        completeSeek (report);

    return move (currentState);
}

RK11D::FunctionProcessorState RK11D::FunctionProcessorStateMachine::transition (Polling&& currentState,
    RKTypes::Function function)
{
    if (function.rkcs.operation == RKTypes::Seek)
    {
        context_->processFunction (function);
        return move (currentState);
    }

    return WaitingForFunction {};
}


// This function is executed in the context of a transition of the
// FunctionProcessorStateMachine which means that the controller mutex
// is locked and register access is allowed.
void RK11D::FunctionProcessorStateMachine::completeSeek (RKTypes::SeekCompleteReport report)
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