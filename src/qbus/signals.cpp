#include "qbus.h"

void Qbus::setSignal (Signal signal, SignalValue value)
{
    controlSignals_[static_cast<size_t> (signal)] = value;
}


bool Qbus::signalIsSet (Signal signal)
{
    switch (controlSignals_[static_cast<size_t> (signal)])
    {
        case SignalValue::False:
            return false;

        case SignalValue::True:
            return true;

        case SignalValue::Cycle:
            controlSignals_[static_cast<size_t> (signal)] = SignalValue::False;
            return true;
    }

    // Satisfying the compiler
    throw string ("Cannot happen");
}