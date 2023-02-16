#include "qbus.h"

void Qbus::setSignal (Signal signal, bool value)
{
    controlSignals_[static_cast<size_t> (signal)] = value;
}


bool Qbus::signalIsSet (Signal signal)
{
    return controlSignals_[static_cast<size_t> (signal)];
}