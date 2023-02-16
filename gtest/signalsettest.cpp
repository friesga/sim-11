#include "signalset/signalset.h"

#include <gtest/gtest.h>

TEST (SignalSetTest, signalCanBeSetAndTested)
{
    SignalSet signals;

    // Signals should be false by default
    EXPECT_TRUE (!signals.isSet (SignalSet::BHALT));

    // Signals can be set and tested
    signals.set (SignalSet::BHALT, true);

    EXPECT_TRUE (signals.isSet (SignalSet::BHALT));
}