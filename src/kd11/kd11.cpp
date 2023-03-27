#include "kd11.h"
#include "qbus/qbus.h"

#include <memory>

using std::make_unique;

// KD11 functions
KD11::KD11 (Qbus* bus)
    :
    BusDevice (bus)
{}

KD11CPU& KD11::cpu ()
{
    return cpu_;
}

void KD11::step ()
{
    switch (cpu_.runState)
    {
        case STATE_HALT:
            // On every entry to ODT a new KD11ODT object is created to make
            // sure it is initialized properly. The Microcomputer and Memories
            // Handbook states: "A / issued immediately after the processor
            // enters ODT mode causes a ?<CR><LF> to be printed because a
            // location has not yet been opened.
            odt_ = make_unique<KD11ODT> (bus_, cpu_);
            odt_->run ();
            break;

        case STATE_RUN:
            cpu_.step ();
            break;

        case STATE_WAIT:
            cpu_.handleTraps ();
            break;
    }
}

