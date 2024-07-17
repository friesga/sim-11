#include "chrono/simulatorclock/simulatorclock.h"
#include "chrono/alarmclock/alarmclock.h"

#include <gtest/gtest.h>
#include <future>
#include <iostream>
#include <shared_mutex>
#include <chrono>

using namespace std::chrono_literals;

class SpyWakeUpCall : public WakeUpCall
{
public:
    SpyWakeUpCall ()
        :
        myId_ {id_++}
    {}

    void ring (uint64_t currentTime) override
    {
        called_ = currentTime;
    }
    void waitFor () override {};
    uint64_t calledAt ()
    {
        return called_;
    }

private:
    size_t id ()
    {
        return myId_;
    }

    uint64_t called_ {0};
    size_t myId_;

    static size_t id_;
};

size_t SpyWakeUpCall::id_ {0};

TEST (SimulatorClock, clockWakesUp)
{
    SpyWakeUpCall spyWakeUpCall;

    // Reset the clock to start at time point 0 so we can check the
    // wake up time.
    SimulatorClock::reset ();

    SimulatorClock::wakeMeAt (SimulatorClock::time_point {SimulatorClock::duration (200ms)}, &spyWakeUpCall);
    SimulatorClock::forwardClock (100ms);

    // Verify the wake up hasn't yet occurred
    EXPECT_EQ (spyWakeUpCall.calledAt (), 0);

    SimulatorClock::forwardClock (200ms);

    // The clock keeps track of simulated time in nanoseconds
    EXPECT_EQ (spyWakeUpCall.calledAt (), 300'000'000);
}

TEST (SimulatorClock, twoWakeUps)
{
    SpyWakeUpCall spyWakeUpCall1, spyWakeUpCall2;

    // Reset the clock to start at time point 0 so we can check the
    // wake up time.
    SimulatorClock::reset ();

    SimulatorClock::wakeMeAt (SimulatorClock::time_point {SimulatorClock::duration (200ms)}, &spyWakeUpCall1);
    SimulatorClock::wakeMeAt (SimulatorClock::time_point {SimulatorClock::duration (400ms)}, &spyWakeUpCall2);

    SimulatorClock::forwardClock (100ms);

    // Verify the wake ups haven't yet occurred
    EXPECT_EQ (spyWakeUpCall1.calledAt (), 0);
    EXPECT_EQ (spyWakeUpCall2.calledAt (), 0);

    SimulatorClock::forwardClock (200ms);

    // Verify that wake up call 1 has occurred and wake up 2 still has to happen.
    //
    // The clock keeps track of simulated time in nanoseconds
    EXPECT_EQ (spyWakeUpCall1.calledAt (), 300'000'000);
    EXPECT_EQ (spyWakeUpCall2.calledAt (), 0);

    SimulatorClock::forwardClock (200ms);
    EXPECT_EQ (spyWakeUpCall1.calledAt (), 300'000'000);
    EXPECT_EQ (spyWakeUpCall2.calledAt (), 500'000'000);
}
