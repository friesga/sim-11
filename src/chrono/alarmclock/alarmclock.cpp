#include "alarmclock.h"
#include "../wakeupcall/alarmwakeupcall/alarmwakeupcall.h"

void AlarmClock::sleepFor (SimulatorClock::duration time)
{
    AlarmWakeUpCall myWakeUpCall;

    // Calculate the TimePoint of wakeup. Actual time is kept in
    // micro seconds.
    SimulatorClock::time_point wakeupTime = 
        SimulatorClock::now () + time;

    // Go to sleep until the indicated time point if the clock 
    // is still running.
    if (SimulatorClock::wakeMeAt (wakeupTime, &myWakeUpCall))
        myWakeUpCall.waitFor ();
}