#include "qbus/qbus.h"

#include <gtest/gtest.h>

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