#include "kd11.h"
#include "qbus/qbus.h"
#include "character/character.h"

#include <memory>
#include <thread>
#include <chrono>

using std::make_unique;
using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::lock_guard;
using std::mutex;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;

using namespace kd11_f;

// The factory power-up mode configuration is mode 0 (get vector at address
// 24 and 26), but we'll set it to Bootstrap as that's more convenient for
// the user.
KD11::KD11 (Qbus* bus)
    :
    BusDevice (bus),
    powerUpMode_ {KD11Config::PowerUpMode::Bootstrap}
{
    subscribeToSignals ();
}

KD11::KD11 (Qbus *bus, shared_ptr<KD11Config> kd11Config)
    :
    BusDevice (bus),
    powerUpMode_ {kd11Config->powerUpMode}
{
    subscribeToSignals ();   
}

// Get notifications on the state of the BHALT and BDCOK signals
void KD11::subscribeToSignals ()
{
    bus_->BHALT().subscribe (bind (&KD11::BHALTReceiver, this, _1));
    bus_->BDCOK().subscribe (bind (&KD11::BDCOKReceiver, this, _1));
}

KD11CPU& KD11::cpu ()
{
    return cpu_;
}

// The BHALT signals halts the processor. If the processor already is in
// ODT mode the signal is ignored.
void KD11::BHALTReceiver (bool signalValue)
{
    if (signalValue)
        cpu_.halt ();
    return;
}

// The BDCOK signal triggers the procesor power-up routine
// 
// The reset is executed when either:
// - The system has been powered up,
// - The user hits the BREAK key and the boot response is configured.
//
// Note that this function will be executed in a different thread from the
// thread in which the CPU is running. Access to the CPU data therefore
// has to be synchronized between the two threads.
void KD11::BDCOKReceiver (bool signalValue)
{
    if (signalValue)
    {
        // Guard against CPU access while an instruction is executed
	    lock_guard<mutex> guard {cpuMutex_};

        powerUpRoutine ();
    }
}

// The reaction on a power-up is configured by the power-up mode. Three
// power-up modes can be selected:
// 1. Trap to location 24/26,
// 2. Place the processor in ODT mode,
// 3. Start the system at the boot address.
// 
// These modes can be selected in the KD11 section of the configuration file.
//
// During the power-up sequence, the processor asserts BINIT L in response to
// a passive (low) power supply-generated BDCOK H signal. When BDCOK H goes
// active (high), the processor terminates BINIT L and the jumper-selected
// power-up sequence is executed.
// (EK-LSI11-TM-003)
//
void KD11::powerUpRoutine ()
{
    cpu_.reset ();
    bus_->BINIT().cycle ();

    switch (powerUpMode_)
    {
        case KD11Config::PowerUpMode::Vector:
            cpu_.setTrap (&powerFail);
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
            break;
    }
}

void KD11::step ()
{
    switch (cpu_.currentCpuState ())
    {
        case CpuState::HALT:
            runODT ();
            break;

        case CpuState::RUN:
        case CpuState::INHIBIT_TRACE:
        case CpuState::WAIT:
        {
            // Guard against CPU access while a BDCOK is received and the power-up
            // is performed.
	        lock_guard<mutex> guard {cpuMutex_};
            cpu_.step ();

            // If BHALT is true the CPU must be single stepped
            if (bus_->BHALT())
                cpu_.halt ();
            break;
        }

        /*
        case CpuState::WAIT:
        {
            // Guard against CPU access while a BDCOK is received and the power-up
            // is performed.
            lock_guard<mutex> guard {cpuMutex_};
            cpu_.handleTraps ();
            break;
        }
        */
    }
}

void KD11::waitForBDCOK ()
{
    while (!bus_->BDCOK())
        sleep_for (milliseconds (10));
}

void KD11::run ()
{
    /*
    while (!bus_->EXIT())
    {
        switch (kd11State_)
        {
            case KD11State::PowerOff:
                waitForBDCOK ();
                break;

            case KD11State::Restart:
                kd11State = powerUpRoutine ();
                break;

		    case KD11State::Running:
                // ToDo: KD11CPU::step() returns true when state is RUN,
                // false otherwise.
                // Traps are to be handled inside the CPU and therefore within
                // the step() function.
                while (bus_->BDCOK() && !bus_->BHALT() &&
                    cpu_.step ());

                if (bus_->BHALT())
                {
                    cpu_.halt ();
                    kd11State_ = KD11State::Halt;
                }
                else if (!bus_->BDCOK())
                    kd11State_ = KD11State::Restart;
                else if (!bus_->BDCOK())
                    kd11State_ = KD11State::Powerfail;
                break;

            case KD11State::Halt:
                // On every entry to ODT a new KD11ODT object is created to make
                // sure it is initialized properly. The Microcomputer and Memories
                // Handbook states: "A / issued immediately after the processor
                // enters ODT mode causes a ?<CR><LF> to be printed because a
                // location has not yet been opened.
                odt_ = make_unique<KD11ODT> (bus_, cpu_);
                odt_->run ();
                // ToDo: cpu_.proceed() hier aanroepen?
                break;

            case KD11State::Powerfail:
                break;
        }
    }
    */
}

// On every entry to ODT a new KD11ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
void KD11::runODT ()
{
    Character character {bus_};

    odt_ = make_unique<KD11ODT> (bus_, cpu_);

    while (true)
    {
        if (character.available ())
        {
            if (!odt_->processCharacter (character.read ()))
                return;
        }

        // if (signalAvailable ())
        //     return createEventFromSignal ();

        // Neither a character nor a bus signal available
        sleep_for (std::chrono::milliseconds (50));
    }
}

