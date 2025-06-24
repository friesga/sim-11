#include "rk11d.h"

#include <exception>
#include <iostream>
#include <latch>

using std::cerr;
using std::latch;

RK11D::PollStateMachine::PollStateMachine (RK11D* context)
    :
    context_ {context}
{}

void RK11D::hardwarePoll ()
try
{
    PollEvent event;

    while (pollEventQueue_.waitAndPop (event))
        pollStateMachine_->dispatch (event);
}
catch (const std::exception& ex)
{
    cerr << "RK11D::hardwarePoll exception: " << ex.what () << '\n';
}

// The hardware poll is started by a StartPoll event issued by the function
// processor when it receives a Seek function.
RK11D::State RK11D::PollStateMachine::transition (Off&&, StartPoll)
{
    return Active {};
}

// On a seek completion reported by an RK05 drive, processing of seek
// completions starts. Seek completions are reported as SeekCompleteReport's
// pushed to the seekCompleteQueue_.
RK11D::State RK11D::PollStateMachine::transition (Active&&, SeekComplete)
{
    return Processing {};
}

// The polling eventually will be stopped by the function processor.
RK11D::State RK11D::PollStateMachine::transition (Active&&, StopPoll)
{
    return Off {};
}

// Seek completions are processed until no completions are no longer available
// or a PollEvent is waiting to be processed. This event should be a StopPoll.
// For every seek completion an interrupt is requested. When the interrupt is
// granted the next seek completion (if available) can be requested.
void RK11D::PollStateMachine::entry (Processing)
{
    while (context_->seekCompleteQueue_.size () > 0 &&
        context_->pollEventQueue_.empty ())
    {
        latch interruptRequestGranted {1};

        context_->bus_->requestInterrupt (TrapPriority::BR5, 5, 0, context_->vector_,
            [&] {
                // Guard against controller register access from other threads
                std::lock_guard<std::mutex> guard {context_->controllerMutex_};

                RKTypes::SeekCompleteReport report =
                    context_->seekCompleteQueue_.first ();

                context_->seekCompleteQueue_.tryPop (report);
                context_->selectedDrive_ = report.driveId;
                context_->rker_.value = report.rker.value;
                interruptRequestGranted.count_down ();
            });

        interruptRequestGranted.wait ();
    }

    if (context_->pollEventQueue_.size () > 0)
        return;

    if (context_->seekCompleteQueue_.empty ())
        context_->pollEventQueue_.push (ProcessingFinished {});
}

// Another seek completion has arrived while processing seek completions
RK11D::State RK11D::PollStateMachine::transition (Processing&&, SeekComplete)
{
    return Processing {};
}

// When no seek completions are available anymore the polling returns to
// the Active state, awaiting further seek completions.
RK11D::State RK11D::PollStateMachine::transition (Processing&&, ProcessingFinished)
{
    return Active {};
}

// The polling eventually will be stopped by the function processor.
RK11D::State RK11D::PollStateMachine::transition (Processing&&, StopPoll)
{
    return Off {};
}
