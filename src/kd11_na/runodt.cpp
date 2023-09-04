#include "kd11_na.h"

using namespace kd11_na;

using std::make_unique;
using std::this_thread::sleep_for;

// On every entry to ODT a new KD11ODT object is created to make
// sure it is initialized properly. The Microcomputer and Memories
// Handbook states: "A / issued immediately after the processor
// enters ODT mode causes a ?<CR><LF> to be printed because a
// location has not yet been opened.
//
// processCharacter() will return false when it cannot process characters
// anymore because either a Proceed or Go command was entered.
void KD11_NA::runODT ()
{
    OperatorConsoleAccess console_ {bus_};
    Event haltEvent {};

    odt_ = make_unique<KD11ODT> (bus_, cpu_, 
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