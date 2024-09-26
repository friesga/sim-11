#include "rl01_02.h"
#include "rlv12/rlv12const.h"

RL01_02::StateMachine::StateMachine (RL01_02* context,
    SimulatorClock::duration spinUpTime)
    : 
    context_ {context},
    spinUpTime_ {spinUpTime}
{}

RL01_02::State RL01_02::StateMachine::transition (Initial&&, SpinUpTime0)
{
    return LockOn {};
}

// On a transition to the Seeking state a time is started for the given 
// seek time. After passage of that time the ring() function is called.
// A more appropriate place for the start of the timer would be on entry of
// the Seeking state but the required seek time is not available in the
// entry() function of that state.
RL01_02::State RL01_02::StateMachine::transition (LockOn&&,
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
    return LockOn {};
}



// This function is executed when the started timer elapses. It generates
// a timer event which will then be processed by the state machine.
void RL01_02::StateMachine::ring (uint64_t currentTime)
{
    context_->eventQueue_.push (TimeElapsed {});
    context_->startCommand_.notify_one ();
}