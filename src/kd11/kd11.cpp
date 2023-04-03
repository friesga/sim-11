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
    bdcokSubscriptionKey_ = bus_->subscribe (Qbus::Signal::BDCOK, 
        bind (&KD11::BDCOKReceiver, this, _1, _2));
}

KD11CPU& KD11::cpu ()
{
    return cpu_;
}

// The BHALT signals halts the processor. If the processor already is in
// ODT mode the signal is ignored.
void KD11::BHALTReceiver (Qbus::Signal signal, 
		Qbus::SignalValue signalValue)
{
    cpu_.halt ();
    return;
}

// The BDCOK signal triggers a bus and therefore a KD11 reset.
// 
// The reset is executed when either:
// - The system has been powered up,
// - The user hits the BREAK key and the boot response is configured,
// - A RESET instruction is executed.
//
// The reaction on a reset is configured by the power-up mode. Three power-up
// modes can be selected:
// 1. Trap to location 24/26,
// 2. Place the processor in ODT mode,
// 3. Start the system at the boot address.
// 
// These modes can be selected in the KD11 section of the configuration file.

void KD11::BDCOKReceiver (Qbus::Signal signal, Qbus::SignalValue signalValue)
{
    switch (powerUpMode)
    {
        case KD11Config::PowerUpMode::Vector:
            break;

        case KD11Config::PowerUpMode::ODT:
            // Halt the processor (if it isn't already halted). This will
            // place the processor in ODT mode on the next execution of
            // KD11::step(). If the processor already is in ODT mode the
            // signal is ignored and this is a no-operation.
            cpu_.halt ();
            break;

        case KD11Config::PowerUpMode::Bootstrap:
            // If an ODT instance is running stop it. Then start the processor
            // at the standard boot address
            if (odt_ != nullptr)
                odt_->stop ();
            cpu_.start (KD11CPU::bootAddress);

            // ToDo: Clearing the PSW should be part of the KD11CPU reset?
            cpu_.setPSW (0);

            // Clearing the interrupts belongs in the Qbus reset
            bus_->clearInterrupts ();
            break;
    }

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

