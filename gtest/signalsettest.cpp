#include "qbus/qbus.h"

#include <gtest/gtest.h>
#include <iostream>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::cout;

class Subscriber
{
public:
    Subscriber (Qbus *bus);
    void signalReceiver (Qbus::Signal signal, Qbus::SignalValue signalValue);
    void sendSignalToOthers ();
    void sendSignalToAll ();
    size_t signalsReceived ();
 
private:
    Qbus *bus_;
    Qbus::SubscriberKey ourKey_;
    size_t signalCount_;
};

// Constructor
// Subscribe to the bus signals we have to be informed of
inline Subscriber::Subscriber (Qbus *bus)
    :
    bus_ {bus},
    ourKey_ {0},
    signalCount_ {0}
{
    ourKey_ = bus_->subscribe (Qbus::Signal::BDCOK, 
        bind (&Subscriber::signalReceiver, this, _1, _2));
}

inline void Subscriber::signalReceiver (Qbus::Signal signal, 
    Qbus::SignalValue signalValue)
{
    ++signalCount_;
}

inline void Subscriber::sendSignalToOthers ()
{
    bus_->setSignal (Qbus::Signal::BDCOK, Qbus::SignalValue::True, ourKey_);
}

inline void Subscriber::sendSignalToAll ()
{
    bus_->setSignal (Qbus::Signal::BDCOK, Qbus::SignalValue::True);
}

inline size_t Subscriber::signalsReceived ()
{
    return signalCount_;
}

TEST (SignalSetTest, signalCanBeSetAndTested)
{
    Qbus bus;

    // Signals should be false by default
    EXPECT_TRUE (!bus.signalIsSet (Qbus::Signal::BHALT));

    // Signals can be set and tested
    bus.setSignal (Qbus::Signal::BHALT, Qbus::SignalValue::True);

    EXPECT_TRUE (bus.signalIsSet (Qbus::Signal::BHALT));
}


TEST (SignalSetTest, signalCanBeSentToAll)
{
    Qbus bus;
    Subscriber subscriberA (&bus);
    Subscriber subscriberB (&bus);

    subscriberA.sendSignalToAll ();

    // Verify all subscribers are notified
    EXPECT_EQ (subscriberA.signalsReceived (), 1);
    EXPECT_EQ (subscriberB.signalsReceived (), 1);
}

TEST (SignalSetTest, signalCanBeSentToOthers)
{
    Qbus bus;
    Subscriber subscriberA (&bus);
    Subscriber subscriberB (&bus);

    subscriberA.sendSignalToOthers ();

    // Verify the signal sender is not notified
    EXPECT_EQ (subscriberA.signalsReceived (), 0);
    EXPECT_EQ (subscriberB.signalsReceived (), 1);
}