#include "rk05.h"

#include <variant>

using std::chrono::duration;
using std::bind;
using std::get;

//
// The state machine uses two different clocks for its timing:
// - The SimulatorClock for timing of the seek commands,
// - The AsyncTimer for timing of the spin and down times.
//
// The AsyncTimer is based on real time so the actual spin up time
// corresponds with the specified spin up time.
//
RK05::StateMachine::StateMachine (RK05* context,
    std::chrono::seconds spinUpTime)
    :
    context_ {context},
    spinUpTime_ {spinUpTime}
{
}

RK05::StateMachine::~StateMachine ()
{
    // Cancel a possibly running spin up/down timer
    spinUpDownTimer_.cancel (&timerId_);
}

RK05::State RK05::StateMachine::transition (Initial&&, Start)
{
    return PoweredOff {};
}

void RK05::StateMachine::entry (PoweredOff)
{
    context_->pwrIndicator_->show (Indicator::State::Off);
    context_->loadIndicator_->show (Indicator::State::Off);
    context_->rdyIndicator_->show (Indicator::State::Off);
    context_->oncylIndicator_->show (Indicator::State::Off);
    context_->wtprotIndicator_->show (Indicator::State::Off);
}

// When the RUN switch is in the upper position the drive has to
// spin up. If the spin up time is zero the drive is immediately locked
// on cylinder 0, else the drive spin up is started. In case the RUN switch
// is in the lower position the drive will be in the Unloaded state.
RK05::State RK05::StateMachine::transition (PoweredOff&&, PowerOn)
{
    return poweringOn ();
}

void RK05::StateMachine::entry (Unloaded)
{
    context_->loadIndicator_->show (Indicator::State::On);
}

RK05::State RK05::StateMachine::transition (Unloaded&&, PowerOff)
{
    return PoweredOff {};
}

// If the RUN/LOAD button is pressed, the state machine transitions to the
// SpinningUp state. The LOAD indicator extinguishes and a spin up
// timer is started.
RK05::State RK05::StateMachine::transition (Unloaded&&, SpinUp)
{
    context_->loadIndicator_->show (Indicator::State::Off);
    return SpinningUp {};
}

// This state can be entered from PoweredOff, EmergencyShutdown and from
// Unloaded.
void RK05::StateMachine::entry (SpinningUp)
{
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_, &timerId_);
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

RK05::State RK05::StateMachine::transition (SpinningUp&&, PowerOff)
{
    return EmergencyShutdown {};
}

// The ONCYL light indicates the drive is locked on a cylinder
void RK05::StateMachine::entry (LockedOn)
{
    context_->oncylIndicator_->show (Indicator::State::On);
    context_->driveStatus_.readWriteSeekReady = 1;
}

void RK05::StateMachine::exit (variantFsm::TagType<LockedOn>)
{
    context_->oncylIndicator_->show (Indicator::State::Off);
    context_->driveStatus_.readWriteSeekReady = 0;
}

void RK05::StateMachine::entry (Seeking)
{}

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

    context_->driveStatus_.readWriteSeekReady = 1;
    context_->oncylIndicator_->show (Indicator::State::On);

    return LockedOn {};
}

// The operator pressed the RUN/LOAD button and the drive has to spin down.
// In the real world this would take approximately 30 seconds. We'll state
// it takes half the time of the spin up time.
RK05::State RK05::StateMachine::transition (LockedOn&&, SpinDown)
{
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_, &timerId_);
    return SpinningDown {};
}

RK05::State RK05::StateMachine::transition (LockedOn&&, PowerOff)
{
    return EmergencyShutdown {};
}

RK05::State RK05::StateMachine::transition (Seeking&&, SpinDown)
{
    spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningDown {};
}

// EmergencyShutdown will start a spin down timer and extinguish all
// indicators.
//
RK05::State RK05::StateMachine::transition (Seeking&&, PowerOff)
{
    return EmergencyShutdown {};
}

void RK05::StateMachine::entry (SpinningDown)
{
    context_->driveStatus_.driveReady = 0;
    context_->rdyIndicator_->show (Indicator::State::Off);
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

RK05::State RK05::StateMachine::transition (SpinningDown&&, PowerOff)
{
    return EmergencyShutdown {};
}

// When AC Low occurs, the drive finishes reading/writing the current sector,
// then initiates a normal head-retract and unload cycle. If a total power
// loss occurs before the heads are completely retracted, the safety relay is
// de-energized to retract the heads under battery power (emergency retract).
// RK05 disk drive maintenance manual (DEC-00-HRK05-C-D), par. 3.3.13.
// 
// This state can be entered from several states and possibly a spin up
// or spin down timer is running. If no such timer is active start one.
void RK05::StateMachine::entry (EmergencyShutdown)
{
    context_->pwrIndicator_->show (Indicator::State::Off);
    context_->loadIndicator_->show (Indicator::State::Off);
    context_->rdyIndicator_->show (Indicator::State::Off);
    context_->oncylIndicator_->show (Indicator::State::Off);
    context_->wtprotIndicator_->show (Indicator::State::Off);

    if (!spinUpDownTimer_.isRunning (&timerId_))
    {
        spinUpDownTimer_.start (bind (&RK05::StateMachine::spinUpDownTimerExpired,
            this), spinUpTime_ / 2, &timerId_);
    }
}

RK05::State RK05::StateMachine::transition (EmergencyShutdown&&, TimeElapsed)
{
    return PoweredOff {};
}

RK05::State RK05::StateMachine::transition (EmergencyShutdown&&, PowerOn)
{
    return poweringOn ();
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

// This function is called from transitions from PoweredOff and
// EmergencyShutdown on a PowerOn event
RK05::State RK05::StateMachine::poweringOn ()
{
    // Cancel a possibly running spin down timer started by EmergencyShutdown
    spinUpDownTimer_.cancel (&timerId_);

    context_->pwrIndicator_->show (Indicator::State::On);

    if (context_->driveStatus_.writeProtectStatus == 1)
        context_->wtprotIndicator_->show (Indicator::State::On);
    else
        context_->wtprotIndicator_->show (Indicator::State::Off);

    if (get<Button::TwoPositionsState> (context_->runLoadSwitch_->currentState ()) ==
        Button::TwoPositionsState::Up)
    {
        if (spinUpTime_ == std::chrono::seconds::zero ())
        {
            context_->driveStatus_.driveReady = 1;
            context_->rdyIndicator_->show (Indicator::State::On);
            return LockedOn {};
        }
        else
            return SpinningUp {};
    }
    else
        return Unloaded {};
}