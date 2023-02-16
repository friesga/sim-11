#include "signalset.h"

void SignalSet::set (Signal signal, bool value)
{
    signals_[signal] = value;
}

bool SignalSet::isSet (Signal signal)
{
    return signals_[signal];
}