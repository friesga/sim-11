#include "kd11.h"
#include "qbus/qbus.h"
#include "character/character.h"

#include <memory>
#include <thread>
#include <chrono>
#include <string>

using std::make_unique;
using std::shared_ptr;
using std::bind;
using std::placeholders::_1;
using std::lock_guard;
using std::mutex;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::string;

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
    signalQueue_.push (Halt {});
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
    signalQueue_.push (PowerOk {});
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
// power-up sequence is executed. (EK-LSI11-TM-003)
// 
// The BDCOK and BPOK signals are replaced by the PowerOk Signal. 
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
            cpu_.setTrap (&powerFail);
            return Running {};

        case KD11Config::PowerUpMode::ODT:
            // Halt the processor (if it isn't already halted). This will
            // place the processor in ODT mode on the next execution of
            // KD11::step(). If the processor already is in ODT mode the
            // signal is ignored and this is a no-operation.
            cpu_.halt ();
            return Halted {};

        case KD11Config::PowerUpMode::Bootstrap:
            // Start the processor at the standard boot address
            cpu_.start (KD11CPU::bootAddress);
            return Running {};
    }

    // Satisfying the compiler
    throw string ("Unknown PowerUpMode");
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
    }
}

void KD11::waitForBDCOK ()
{
    while (!bus_->BDCOK())
        sleep_for (milliseconds (10));
}

void KD11::run ()
{
    Event event;

    // ToDo: Replace by transition to ExitPoint?
    while (!bus_->EXIT())
    {
        // Read a character from the console, create the appropriate event
        // from it and process that event
        signalQueue_.waitAndPop (event);
        dispatch (event);
    }
}

bool KD11::signalAvailable ()
{
    return signalQueue_.size () > 0;
}

// On every entry to ODT a new KD11ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
//
// processCharacter() will return false when it cannot process characters
// anymore because either a Proceed or Go command was entered.
void KD11::runODT ()
{
    Character character {bus_};

    odt_ = make_unique<KD11ODT> (bus_, cpu_);

    while (!signalAvailable ())
    {
        if (character.available ())
        {
            if (!odt_->processCharacter (character.read ()))
            {
                signalQueue_.push (Start {});
                break;
            }
        }

        // Neither a character nor a bus signal available
        sleep_for (std::chrono::milliseconds (50));
    }
}

