#include "qbus.h"

// Add the given subscriber to the subscribs for the given signal
void Qbus::subscribe (Signal signal, Subscriber subscriber)
{
    signalSubscribers_[static_cast<size_t> (signal)].push_back (subscriber);
}

void Qbus::setSignal (Signal signal, SignalValue value)
{
    signalValues_[static_cast<size_t> (signal)] = value;

    // Notify all subscribers to the signal
    for (Subscriber subscriber : signalSubscribers_[static_cast<size_t> (signal)])
        subscriber (signal, value);
}

// Return the Signal's value.
// 
// The SignalValue::Cycle is a special value as the first time it is read as
// True and following read's return the calue False.
bool Qbus::signalIsSet (Signal signal)
{
    switch (signalValues_[static_cast<size_t> (signal)])
    {
        case SignalValue::False:
            return false;

        case SignalValue::True:
            return true;

        case SignalValue::Cycle:
            signalValues_[static_cast<size_t> (signal)] = SignalValue::False;
            return true;
    }

    // Satisfying the compiler
    throw string ("Cannot happen");
}