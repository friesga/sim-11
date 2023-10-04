#include "kd11_na.h"
#include "qbus/qbus.h"
#include "operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>

using std::shared_ptr;
using std::make_unique;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KD11_NA::KD11_NA (Qbus* bus)
    :
    bus_ (bus),
    powerUpMode_ {KD11_NAConfig::PowerUpMode::Bootstrap},
    kd11Running_ {true},
    startAddress_ {stdBootAddress}
{
    stateMachine_ = make_unique<StateMachine> (this);
    subscribeToSignals ();
}

KD11_NA::KD11_NA (Qbus *bus, shared_ptr<KD11_NAConfig> kd11_naConfig)
    :
    KD11_NA (bus)
{
    powerUpMode_ = kd11_naConfig->powerUpMode;
}

KD11_NA::~KD11_NA ()
{
    signalEventQueue_.push (Exit {});
    kd11Thread_.join ();
}

KD11_NA_Cpu& KD11_NA::cpu ()
{
    return cpu_;
}

// Run the KD11_NA state machine
void KD11_NA::run ()
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

void KD11_NA::start ()
{
    kd11Thread_ = std::thread ([&, this] {this->run ();});
}

// Start the KD11_NA state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KD11_NA::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

bool KD11_NA::signalAvailable ()
{
    return signalEventQueue_.size () > 0;
}


