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
    uint64_t called_ {0};
};

TEST (SimulatorClock, clockWakesUp)
{
    SimulatorClock simulatorClock;
    SpyWakeUpCall spyWakeUpCall;

    simulatorClock.wakeMeAt (SimulatorClock::time_point {SimulatorClock::duration (200ms)}, &spyWakeUpCall);
    simulatorClock.forwardClock (100ms);
    EXPECT_EQ (spyWakeUpCall.calledAt (), 0);

    simulatorClock.forwardClock (200ms);
    EXPECT_EQ (spyWakeUpCall.calledAt (), 300000);
}
