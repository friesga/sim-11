#include "kd11.h"
#include "qbus/qbus.h"
#include "operatorconsoleaccess/operatorconsoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>

using std::shared_ptr;

using namespace kd11_na;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KD11::KD11 (Qbus* bus)
    :
    BusDevice (bus),
    powerUpMode_ {KD11Config::PowerUpMode::Bootstrap},
    kd11Running_ {true},
    startAddress_ {stdBootAddress}
{
    subscribeToSignals ();
}

KD11::KD11 (Qbus *bus, shared_ptr<KD11Config> kd11Config)
    :
    KD11 (bus)
{
    powerUpMode_ = kd11Config->powerUpMode;
}

KD11::~KD11 ()
{
    signalEventQueue_.push (Exit {});
    kd11Thread_.join ();
}


KD11CPU& KD11::cpu ()
{
    return cpu_;
}


// Run the KD11 state machine
void KD11::run ()
{
    Event event;

    while (kd11Running_)
    {
        // Read a character from the console, create the appropriate event
        // from it and process that event
        signalEventQueue_.waitAndPop (event);
        dispatch (event);
    }
}

void KD11::start ()
{
    kd11Thread_ = std::thread ([&, this] {this->run ();});
}

// Start the KD11 state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KD11::start (u16 startAddress)
{
    startAddress_ = startAddress;
    start ();
}

bool KD11::signalAvailable ()
{
    return signalEventQueue_.size () > 0;
}


