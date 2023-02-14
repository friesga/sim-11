#include "kd11.h"
#include "qbus/qbus.h"

// KD11 functions
KD11::KD11 (QBUS* bus)
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
            odt_.step ();
            break;

        case STATE_RUN:
            cpu_.step ();
            if (cpu_.runState == STATE_HALT)
                // Reset the ODT state to ODT_STATE_INIT
                odt_.reset ();
            break;

        case STATE_WAIT:
            cpu_.handleTraps ();
            break;
    }
}

