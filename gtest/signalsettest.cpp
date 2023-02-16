#include "qbus/qbus.h"

#include <gtest/gtest.h>

TEST (SignalSetTest, signalCanBeSetAndTested)
{
    Qbus bus;

    // Signals should be false by default
    EXPECT_TRUE (!bus.signalIsSet (Qbus::Signal::BHALT));

    // Signals can be set and tested
    bus.setSignal (Qbus::Signal::BHALT, true);

    EXPECT_TRUE (bus.signalIsSet (Qbus::Signal::BHALT));
}