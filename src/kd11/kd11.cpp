#include "kd11.h"
#include "qbus/qbus.h"

#include <memory>

using std::make_unique;
using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KD11::KD11 (Qbus* bus)
    :
    BusDevice (bus),
    powerUpMode {KD11Config::PowerUpMode::Bootstrap}
{
    subscribeToSignals ();
}

KD11::KD11 (Qbus *bus, shared_ptr<KD11Config> kd11Config)
    :
    BusDevice (bus),
    powerUpMode {kd11Config->powerUpMode}
{
    subscribeToSignals ();   
}

// Get notifications on the state of the BHALT and BDCOK signals
void KD11::subscribeToSignals ()
{
    bus_->subscribe (Qbus::Signal::BHALT, 
        bind (&KD11::BHALTReceiver, this, _1, _2));
    bus_->subscribe (Qbus::Signal::BDCOK, 
        bind (&KD11::BDCOKReceiver, this, _1, _2));
}

KD11CPU& KD11::cpu ()
{
    return cpu_;
}

void KD11::BHALTReceiver (Qbus::Signal signal, 
		Qbus::SignalValue signalValue)
{
    cpu_.halt ();
    return;
}

// When the BDCOK signal is cycled the power-up function has to be executed
void KD11::BDCOKReceiver (Qbus::Signal signal, Qbus::SignalValue signalValue)
{
    cpu_.start (KD11CPU::bootAddress);

    // ToDo: Clearing the PSW should be part of the KD11CPU reset?
    cpu_.setPSW (0);

    // Clearing the interrupts belongs in the Qbus reset
    bus_->clearInterrupts ();
}

void KD11::step ()
{
    switch (cpu_.currentCpuState ())
    {
        case CpuState::HALT:
            // On every entry to ODT a new KD11ODT object is created to make
            // sure it is initialized properly. The Microcomputer and Memories
            // Handbook states: "A / issued immediately after the processor
            // enters ODT mode causes a ?<CR><LF> to be printed because a
            // location has not yet been opened.
            odt_ = make_unique<KD11ODT> (bus_, cpu_);
            odt_->run ();
            break;

        case CpuState::RUN:
            cpu_.step ();
            break;

        case CpuState::WAIT:
            cpu_.handleTraps ();
            break;
    }
}

