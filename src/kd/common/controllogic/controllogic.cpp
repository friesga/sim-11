#include "controllogic.h"
#include "qbus/qbus.h"
#include "kd/kd11_na/operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>

using std::shared_ptr;
using std::make_unique;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
ControlLogic::ControlLogic (Qbus* bus, CpuData* cpu, 
    KD11_NAConfig::PowerUpMode powerUpMode, u16 startAddress,
    KD11ODT::Creator odtCreator)
    :
    bus_ (bus),
    cpu_ {cpu},
    powerUpMode_ {powerUpMode},
    startAddress_ {startAddress},
    odtCreator_ {odtCreator},
    kd11Running_ {true}
{
    stateMachine_ = make_unique<StateMachine> (this);
    subscribeToSignals ();
}

ControlLogic::~ControlLogic ()
{
    signalEventQueue_.push (Exit {});
}

// Run the ControlLogic state machine
void ControlLogic::run ()
{
    Event event;

    while (kd11Running_)
    {
        // Read a character from the console, create the appropriate event
        // from it and process that event
        signalEventQueue_.waitAndPop (event);
        stateMachine_->dispatch (event);
    }
}

bool ControlLogic::signalAvailable ()
{
    return signalEventQueue_.size () > 0;
}
