#include "signal.h"

Signal::Signal (bool initialValue)
    :
    value_ {initialValue},
    subscribers_ {}
{}

Signal::Signal ()
    :
    Signal (false)
{}

// Set the signal to the specified value. If that value differs
// from the set value, the subscribers are notified.
void Signal::set (bool value, SubscriberKey sender)
{
    // Guard against simultaneous setting in different threads
    lock_guard<mutex> guard {signalMutex_};

    value_ = value;
    notifyObservers (sender);
}

bool Signal::isTrue ()
{
    return value_ == true;
}

// Cycle the signal. The current value is inverted, all subscribers are
// notified of the state change, then the value is inverted back to its
// original value and all subscribers are notified once again.
void Signal::cycle (SubscriberKey sender)
{
    set (!value_, sender);
    set (!value_, sender);
}

// Add the given subscriber to the subscribers for this signal. Return
// a subscriber key (i.e. an index into the vector of subscribers). This key
// can be used to supress sending a signal to its originator. The first
// subscriber will receive the SubscriberKey 1, so SubscriberKey 0 can be used
// to indicate that no supression has to take place and the signal should also
// be send to its originator.
Signal::SubscriberKey Signal::subscribe (Subscriber subscriber)
{
    subscribers_.push_back (subscriber);
    return subscribers_.end () - subscribers_.begin ();
}

// Notify the subscribers to the signal except for the sender if one is
// specified
void Signal::notifyObservers (SubscriberKey sender)
{
    for (auto iter = subscribers_.begin(); iter < subscribers_.end (); ++iter)
    {
        SubscriberKey thisKey = iter - subscribers_.begin() + 1;
        if (sender == 0 || sender != thisKey)
            (*iter) (value_);
    }
}