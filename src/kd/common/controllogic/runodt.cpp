#include "controllogic.h"
#include "kd/kd11_na/kd11_na.h"

#include <functional>

using std::unique_ptr;
using std::make_unique;
using std::this_thread::sleep_for;
using std::function;
using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// On every entry to ODT a new KD11_NA_ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
//
// processCharacter() will return false when it cannot process characters
// anymore because either a Proceed or Go command was entered.
void ControlLogic::runODT ()
{
    OperatorConsoleAccess console_ {bus_};
    Event haltEvent {};

    // Create a fresh ODT object. The function to create the object is passed
    // to the ControlLogic when it is constructed.
    odt_ = odtCreator_ (bus_, cpuData_, cpuControl_, mmu_,
        make_unique<OperatorConsoleAccess> (bus_));

    while (true)
    {
        if (signalAvailable ())
        {
            // If the signal is a HALT remove the event and ignore it
            // so we don't leave the Halted state.
            if (signalIsOfType<Halt> ())
                signalEventQueue_.tryPop (haltEvent);
            else
                return;
        }

        if (console_.available ())
        {
            if (!odt_->processCharacter (console_.read ()))
            {
                // The user issued a Proceed or Go command so start the CPU
                signalEventQueue_.push (Start {});
                return;
            }
        }

        // Neither a character nor a bus signal available
        sleep_for (std::chrono::milliseconds (50));
    }
}