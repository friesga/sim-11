#include "kd11.h"
#include "qbus/qbus.h"
#include "consoleaccess/consoleaccess.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>

using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::lock_guard;
using std::chrono::milliseconds;
using std::holds_alternative;

using namespace kd11_f;

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

// Get notifications on the state of the signals
void KD11::subscribeToSignals ()
{
    bus_->BHALT().subscribe (bind (&KD11::BHALTReceiver, this, _1));
    bus_->BPOK().subscribe (bind (&KD11::BPOKReceiver, this, _1));
    bus_->EXIT().subscribe (bind (&KD11::ExitReceiver, this, _1));
    bus_->RESTART().subscribe (bind (&KD11::RestartReceiver, this, _1));
}

KD11CPU& KD11::cpu ()
{
    return cpu_;
}

// The BHALT signal halts the processor. 
void KD11::BHALTReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Halt {});
}

// The BPOK signal triggers the procesor power-up routine
// 
// The reset is executed when either:
// - The system has been powered up,
// - The user hits the BREAK key and the boot response is configured.
//
// Note that this function will be executed in a different thread from the
// thread in which the CPU is running. Access to the signal event is 
// synchronized via the ThreadSafeQueue.
void KD11::BPOKReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (BPOK_high {});
    else
        signalEventQueue_.push (BPOK_low {});
}


void KD11::ExitReceiver (bool signalValue)
{
    signalEventQueue_.push (Exit {});
}

void KD11::RestartReceiver (bool signalValue)
{
    if (signalValue)
        signalEventQueue_.push (Reset {});
}

// The reaction on a power-up is configured by the power-up mode. Three
// power-up modes can be selected:
// 1. Trap to location 24/26,
// 2. Place the processor in ODT mode,
// 3. Start the system at the boot address.
// 
// These modes can be selected in the KD11 section of the configuration file.
// See Table 11-4 (Console Power-Up Printout (or Display) in the LSI-11,
// PDP-11/03 user's manual (EK-LSI11-TM-003) for the expected reaction on
// power-up.
//
// During the power-up sequence, the processor asserts BINIT L in response to
// a passive (low) power supply-generated BDCOK H signal. When BDCOK H goes
// active (high), the processor terminates BINIT L and the jumper-selected
// power-up sequence is executed. (EK-LSI11-TM-003)
//
// The function will return the state to transition to.
//
kd11_f::State KD11::powerUpRoutine ()
{
    cpu_.reset ();
    bus_->BINIT().cycle ();

    switch (powerUpMode_)
    {
        case KD11Config::PowerUpMode::Vector:
            cpu_.loadTrapVector (&powerFail);
            cpu_.proceed ();

            // If BHALT is set immediately transition to the Halted state,
            // before even one instruction is executed cf Table 11-4.
            if (bus_->BHALT ())
                return Halted {};

            return Running {};

        case KD11Config::PowerUpMode::ODT:
            // Halt the processor (if it isn't already halted). This will
            // place the processor in ODT mode on the next execution of
            // KD11::step(). If the processor already is in ODT mode the
            // signal is ignored and this is a no-operation.
            cpu_.halt ();
            return Halted {};

        case KD11Config::PowerUpMode::Bootstrap:
            // Start the processor at the start address. This address is
            // either the standard boot address or an address determined by
            // a loaded file in absolute loader format.
            cpu_.start (startAddress_);

            // If BHALT is set immediately transition to the Halted state,
            // before even one instruction is executed cf Table 11-4.
            if (bus_->BHALT ())
                return Halted {};

            return Running {};
    }

    // Satisfying the compiler
    throw string ("Unknown PowerUpMode");
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

// Run the KD11 state machine, starting the CPU at the given address. This
// address supersedes the standard boot address.
void KD11::run (u16 startAddress)
{
    startAddress_ = startAddress;
    run ();
}

bool KD11::signalAvailable ()
{
    return signalEventQueue_.size () > 0;
}



