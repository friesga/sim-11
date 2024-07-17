#ifndef _ALARMCLOCK_H_
#define _ALARMCLOCK_H_

#include "../simulatorclock/simulatorclock.h"
#include "../../threadsafecontainers/threadsafeprioqueue.h"

class AlarmClock
{
public:
    void sleepFor (SimulatorClock::duration time);
    void sleepUntil (SimulatorClock::time_point wakeUpTime);
};

#endif // _ALARMCLOCK_H_