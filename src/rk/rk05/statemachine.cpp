#include "rk05.h"

using std::chrono::duration;
using std::bind;

//
// The state machine uses two different clocks for its timing:
// - The SimulatorClock for timing of the seek commands,
// - The AsyncTimer for timing of the spin and down times.
//
// The AsyncTimer is based on real time so the actual spin up time
// corresponds with the specified spin up time.
//
RK05::StateMachine::StateMachine (RK05* context,
    duration<int, std::ratio<1, 1>> spinUpTime)
    :
    context_ {context},
    spinUpTime_ {spinUpTime}
{
}

// From the Initial state the state machine either transitions to the SpunUp
// state if a spin up time of 0 seconds is specified or to the SpinningUp state
// if a spin up time greater than zero is given.
RK05::State RK05::StateMachine::transition (Initial&&, SpunUp)
{
    context_->driveStatus_.driveReady = 1;

    context_->loadIndicator_->show (Indicator::State::Off);
    context_->rdyIndicator_->show (Indicator::State::On);

    return LockedOn {};
}

RK05::State RK05::StateMachine::transition (Initial&&, SpunDown)
{
    return Unloaded {};
}

void RK05::StateMachine::entry (Unloaded)
{
    context_->loadIndicator_->show (Indicator::State::On);
    context_->rdyIndicator_->show (Indicator::State::Off);
    context_->oncylIndicator_->show (Indicator::State::Off);
}

// If the RUN/LOAD button is pressed, the state machine transitions to the
// SpinningUp state. The LOAD indicator extinguishes and a spin up
// timer is started.
RK05::State RK05::StateMachine::transition (Unloaded&&, SpinUp)
{
    context_->loadIndicator_->show (Indicator::State::Off);

    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_, &timerId_);

    return SpinningUp {};
}

void RK05::StateMachine::entry (SpinningUp)
{
}

// The spin up timer fires and the drive is spun up and locked on cylinder 0.
RK05::State RK05::StateMachine::transition (SpinningUp&&, TimeElapsed)
{
    context_->driveStatus_.driveReady = 1;

    context_->rdyIndicator_->show (Indicator::State::On);

    return LockedOn {};
}

RK05::State RK05::StateMachine::transition (SpinningUp&&, SpinDown)
{
    spinUpDownTimer_.cancel (&timerId_);
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningDown {};
}

// The ONCYL light indicates the drive is locked on a cylinder
void RK05::StateMachine::entry (LockedOn)
{
    context_->oncylIndicator_->show (Indicator::State::On);
}

void RK05::StateMachine::exit (variantFsm::TagType<LockedOn>)
{
    context_->oncylIndicator_->show (Indicator::State::Off);
}

void RK05::StateMachine::entry (Seeking)
{
    
}

// On a transition to the Seeking state a time is started for the given 
// seek time. After passage of that time the ring() function is called.
// A more appropriate place for the start of the timer would be on entry of
// the Seeking state but the required seek time is not available in the
// entry() function of that state.
RK05::State RK05::StateMachine::transition (LockedOn&&, 
    SeekCommand seekCommand)
{
    SimulatorClock::wakeMeAt (SimulatorClock::now () + seekCommand.seekTime,
        this);

    return Seeking {seekCommand.seekCompleted};
}

// This function is executed when a seek is completed.
RK05::State RK05::StateMachine::transition (Seeking&& currentState, TimeElapsed)
{
    if (currentState.seekCompleted != nullptr)
        currentState.seekCompleted ();

    // context_->seekCompleted ();
    return LockedOn {};
}

// The operator pressed the RUN/LOAD button and the drive has to spin down.
// In the real world this would take approximately 30 seconds. We'll state
// it takes half the time of the spin up time.
RK05::State RK05::StateMachine::transition (LockedOn&&, SpinDown)
{
    context_->driveStatus_.driveReady = 0;
    context_->rdyIndicator_->show (Indicator::State::Off);
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_, &timerId_);
    return SpinningDown {};
}

RK05::State RK05::StateMachine::transition (Seeking&&, SpinDown)
{
    context_->driveStatus_.driveReady = 0;
    context_->rdyIndicator_->show (Indicator::State::Off);
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningDown {};
}

void RK05::StateMachine::entry (SpinningDown)
{
}

RK05::State RK05::StateMachine::transition (SpinningDown&&, TimeElapsed)
{
    context_->loadIndicator_->show (Indicator::State::On);
    return Unloaded {};
}

RK05::State RK05::StateMachine::transition (SpinningDown&&, SpinUp)
{
    spinUpDownTimer_.cancel (&timerId_);
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningUp {};
}

// This or the following function is executed when a started timer elapses.
// It generates a timer event which will then be processed by the state
// machine.
void RK05::StateMachine::spinUpDownTimerExpired ()
{
    lock_guard<mutex> lock {context_->driveMutex_};
    context_->eventQueue_.push (TimeElapsed {});
    context_->startCommand_.notify_one ();
}

// This version of the timer expired function is used by the SimulatorClock.
void RK05::StateMachine::ring (uint64_t currentTime)
{
    spinUpDownTimerExpired ();
}