#include "rl01_02.h"
#include "disk/rlv12/rlv12const.h"

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
RL01_02::StateMachine::StateMachine (RL01_02* context,
    duration<int, std::ratio<1, 1>> spinUpTime)
    : 
    context_ {context},
    spinUpTime_ {spinUpTime}
{}

// From the Initial state the state machine can transition to the following
// states:
// - CatridgeUnloaded, if the unit configuration doesn't specify a filename,
// - LockedOn, if a file name has been specified in the unit configuration
//   and a spin up time of 0 seconds is specified,
// - SpindleStopped, if a file name has been specified and a spin up time
//   greater than zero is given.

RL01_02::State RL01_02::StateMachine::transition (Initial&&, UnloadedCartridge)
{
    context_->driveStatus_ = 
        RLV12const::MPR_GS_CoverOpen | RLV12const::MPR_GS_BrushHome;
    return CartridgeUnloaded {};
}

RL01_02::State RL01_02::StateMachine::transition (Initial&&, SpunUp)
{
    context_->loadButton_->show (Indicator::State::Off);
    context_->loadButton_->setState (Button::TwoPositionsState::Down);
    context_->readyIndicator_->show (Indicator::State::On);
    context_->driveStatus_ |= RLV12const::MPR_GS_VolumeCheck |
        RLV12const::MPR_GS_BrushHome | RLV12const::MPR_GS_HeadsOut |
        RLV12const::MPR_GS_LockOn;
    context_->setDriveReady ();
    return LockedOn {};
}

RL01_02::State RL01_02::StateMachine::transition (Initial&&, SpunDown)
{
    return SpindleStopped {};
}

RL01_02::State RL01_02::StateMachine::transition (CartridgeUnloaded&&, LoadedCartridge)
{
    return SpindleStopped {};
}

void RL01_02::StateMachine::entry (SpindleStopped)
{
    context_->driveStatus_ = RLV12const::MPR_GS_LoadCartridge;
}

RL01_02::State RL01_02::StateMachine::transition (SpindleStopped&&, UnloadedCartridge)
{
    return CartridgeUnloaded {};
}

// If the LOAD button is pressed, the state machine transitions to the
// SpinningUp state. The LOAD indicator extinguishes and a spin up
// timer is started.
RL01_02::State RL01_02::StateMachine::transition (SpindleStopped &&, SpinUp)
{
    context_->loadButton_->show (Indicator::State::Off);
    
    spinUpDownTimer_.start (bind (&RL01_02::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_, &timerId_);

    return SpinningUp {};
}

void RL01_02::StateMachine::entry (SpinningUp)
{
    context_->driveStatus_ = RLV12const::MPR_GS_SpinUp;
}

// The spin up timer fires and the drive is spun up and locked on cylinder 0.
// A Volume Check condition is "set during transition from a head-load state to
// a head-on-track state" (EK-RLV12-TD-001).
RL01_02::State RL01_02::StateMachine::transition (SpinningUp&&, TimeElapsed)
{
    context_->readyIndicator_->show (Indicator::State::On);
    context_->driveStatus_ |= RLV12const::MPR_GS_VolumeCheck |
        RLV12const::MPR_GS_BrushHome | RLV12const::MPR_GS_HeadsOut |
        RLV12const::MPR_GS_LockOn;
    context_->setDriveReady ();
    return LockedOn {};
}

RL01_02::State RL01_02::StateMachine::transition (SpinningUp&&, SpinDown)
{
    spinUpDownTimer_.cancel (&timerId_);
    spinUpDownTimer_.start (bind (&RL01_02::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningDown {};
}

// The READY light indicates the drive is locked on a cylinder
void RL01_02::StateMachine::entry (LockedOn)
{
    context_->readyIndicator_->show (Indicator::State::On);
}

void RL01_02::StateMachine::exit (variantFsm::TagType<LockedOn>)
{
    context_->readyIndicator_->show (Indicator::State::Off);
}

void RL01_02::StateMachine::entry (Seeking)
{
    context_->driveStatus_ = 
        (context_->driveStatus_ & ~RLV12const::MPR_GS_State) |
        RLV12const::MPR_GS_Seek;
}

// On a transition to the Seeking state a time is started for the given 
// seek time. After passage of that time the ring() function is called.
// A more appropriate place for the start of the timer would be on entry of
// the Seeking state but the required seek time is not available in the
// entry() function of that state.
RL01_02::State RL01_02::StateMachine::transition (LockedOn&&,
    SeekCommand seekCommand)
{
    SimulatorClock::wakeMeAt (SimulatorClock::now () + seekCommand.seekTime,
        this);

    return Seeking {};
}

// This function is executed when a seek is completed.
RL01_02::State RL01_02::StateMachine::transition (Seeking&&, TimeElapsed)
{
    context_->driveStatus_ = (context_->driveStatus_ & ~RLV12const::MPR_GS_State) |
        RLV12const::MPR_GS_LockOn;
    context_->setDriveReady ();
    return LockedOn {};
}

// The operator pressed the LOAD button and the drive has to spin down. In the
// real world this would take approximately 30 seconds. We'll state it takes
// half the time of the spin up time.
RL01_02::State RL01_02::StateMachine::transition (LockedOn&&, SpinDown)
{
    context_->readyIndicator_->show (Indicator::State::Off);
    spinUpDownTimer_.start (bind (&RL01_02::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_, &timerId_);
    return SpinningDown {};
}

RL01_02::State RL01_02::StateMachine::transition (Seeking&&, SpinDown)
{
    context_->readyIndicator_->show (Indicator::State::Off);
    spinUpDownTimer_.start (bind (&RL01_02::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningDown {};
}

void RL01_02::StateMachine::entry (SpinningDown)
{
    context_->driveStatus_ = RLV12const::MPR_GS_SpinDown;
}

RL01_02::State RL01_02::StateMachine::transition (SpinningDown&&, TimeElapsed)
{
    context_->loadButton_->show (Indicator::State::On);
    return SpindleStopped {};
}

RL01_02::State RL01_02::StateMachine::transition (SpinningDown&&, SpinUp)
{
    spinUpDownTimer_.cancel (&timerId_);
    spinUpDownTimer_.start (bind (&RL01_02::StateMachine::spinUpDownTimerExpired,
        this), spinUpTime_ / 2, &timerId_);
    return SpinningUp {};
}

// This or the following function is executed when a started timer elapses.
// It generates // a timer event which will then be processed by the state
// machine.
void RL01_02::StateMachine::spinUpDownTimerExpired ()
{
    lock_guard<mutex> lock {context_->driveMutex_};
    context_->eventQueue_.push (TimeElapsed {});
    context_->startCommand_.notify_one ();
}

// This version of the timer expired function is used by the SimulatorClock.
void RL01_02::StateMachine::ring (uint64_t currentTime)
{
    spinUpDownTimerExpired ();
}