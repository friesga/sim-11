#include "alarmclock.h"
#include "../wakeupcall/alarmwakeupcall/alarmwakeupcall.h"

void AlarmClock::sleepFor (SimulatorClock::duration time)
{
    AlarmWakeUpCall myWakeUpCall;

    // Calculate the TimePoint of wakeup. Actual time is kept in
    // micro seconds.
    SimulatorClock::time_point wakeupTime = 
        SimulatorClock::now () + time;

    // Add this time to the timer queue
    SimulatorClock::wakeMeAt (wakeupTime, &myWakeUpCall);

    myWakeUpCall.waitFor ();
}