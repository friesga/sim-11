#include "qbus/qbus.h"

#include <gtest/gtest.h>
#include <iostream>

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;
using std::cout;

struct Subscriber
{
    Subscriber (Qbus *bus);
    void signalReceiver (Qbus::Signal signal, Qbus::SignalValue signalValue);

    size_t signalCount {0};
};

// Constructor
// Subscribe to the bus signals we have to be informed of
inline Subscriber::Subscriber (Qbus *bus)
{
    bus->subscribe (Qbus::Signal::BDCOK, 
        bind (&Subscriber::signalReceiver, this, _1, _2));
}

inline void Subscriber::signalReceiver (Qbus::Signal signal, 
    Qbus::SignalValue signalValue)
{
    ++signalCount;
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

TEST (SignalSetTest, signalCanBeCycled)
{
    Qbus bus;

    // Signals should be false by default
    EXPECT_FALSE (bus.signalIsSet (Qbus::Signal::BHALT));

    // Signal is set to cycle
    bus.setSignal (Qbus::Signal::BHALT, Qbus::SignalValue::Cycle);

    // The first read should return true, the next read should return false
    EXPECT_TRUE (bus.signalIsSet (Qbus::Signal::BHALT));
    EXPECT_FALSE (bus.signalIsSet (Qbus::Signal::BHALT));
}

TEST (SignalSetTest, signalIsReceived)
{
    Qbus bus;
    Subscriber subscriberA (&bus);

    bus.setSignal (Qbus::Signal::BDCOK, Qbus::SignalValue::True);

    EXPECT_TRUE (subscriberA.signalCount == 1);
}