#include "qbus.h"

// Add the given subscriber to the subscribers for the given signal. Return
// a subscriber key (i.e. an index into the vector of subscribers). This key
// can be used to supress sending a signal to its originator. The first
// subscriber will receive the SubscriberKey 1, so SubscriberKey 0 can be used
// to indicate that no supression has to take place and the signal should also
// be send to its originator.
Qbus::SubscriberKey Qbus::subscribe (Signal signal, Subscriber subscriber)
{
    signalSubscribers_[static_cast<size_t> (signal)].push_back (subscriber);

    vector<Subscriber> &subscriberVector = 
        signalSubscribers_[static_cast<size_t> (signal)];
    return subscriberVector.end () - subscriberVector.begin ();
}

// A Signal will be in either one of the two states: False or True. The default
// signal state is false. The value parameter can have an additional value:
// Cycle. When that value is given, the signal is set True and when all
// subsribers are notified the signal is set False again.
void Qbus::setSignal (Signal signal, SignalValue value, SubscriberKey sender)
{
    if (value == SignalValue::Cycle)
        signalValues_[static_cast<size_t> (signal)] = SignalValue::True;
    else
        signalValues_[static_cast<size_t> (signal)] = value;

    // Notify the subscribers to the signal except for the sender if one is
    // specified
    vector<Subscriber> &subscriberVector = 
        signalSubscribers_[static_cast<size_t> (signal)];
    for (auto iter = subscriberVector.begin(); iter < subscriberVector.end (); ++iter)
    {
        SubscriberKey thisKey = iter - subscriberVector.begin() + 1;
        if (sender == 0 || sender != thisKey)
            (*iter) (signal, signalValues_[static_cast<size_t> (signal)]);
    }

    // A Cycle signal value is reset after all subscribers have been notified
    if (value == SignalValue::Cycle)
        signalValues_[static_cast<size_t> (signal)] = SignalValue::False;
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
        case SignalValue::Cycle:
            return true;
    }

    // Satisfying the compiler
    throw string ("Cannot happen");
}