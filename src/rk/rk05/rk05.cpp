#include "rk05.h"

#include <memory>
#include <chrono>

using std::shared_ptr;
using std::make_unique;
using std::chrono::seconds;

RK05::RK05 (Bus* bus, AbstractBusDevice* controller, Window* window,
    shared_ptr<RK05Config> rk05Config)
    : 
    Unit (controller),
    bus_ {bus},
    controller_ {controller}
{
    // In case the constructor is called in the unit tests, the window is
    // not available.
    if (window != nullptr)
        createBezel (window, rk05Config);

    stateMachine_ = make_unique<StateMachine> (this,
        seconds (rk05Config->spinUpTime));

    running_ = true;
    driveThread_ = thread (&RK05::driveThread, this);

    // Immediately lock the drive on cylinder 0 if the spin up time is
    // zero.
    if (rk05Config->spinUpTime == 0)
        stateMachine_->dispatch (SpunUp {});
    else
        stateMachine_->dispatch (SpunDown {});
}

// Finish the drive thread
RK05::~RK05 ()
{
    // Stop a potentionally running drive thread
    if (running_)
    {
        // Wakeup the drive thread with the indication to finish
        running_ = false;
        startCommand_.notify_one ();

        // Wait till the thread has finished
        driveThread_.join ();
    }
}

void RK05::runLoadSwitchClicked (Button::State state)
{
    if (get<Button::TwoPositionsState> (state) == Button::TwoPositionsState::On)
        sendTrigger (SpinUp {});
    else
        sendTrigger (SpinDown {});
}

// This function puts the specified event in the event queue thereby
// triggering the state machine to process the event.
void RK05::sendTrigger (Event event)
{
    unique_lock<mutex> lock {driveMutex_};
    eventQueue_.push (event);
    startCommand_.notify_one ();
}

void RK05::wtprotSwitchClicked (Button::State state)
{
}