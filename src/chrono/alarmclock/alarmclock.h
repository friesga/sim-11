#ifndef _ALARMCLOCK_H_
#define _ALARMCLOCK_H_

#include "../simulatorclock/simulatorclock.h"
#include "../../threadsafecontainers/threadsafeprioqueue.h"

class AlarmClock
{
public:
    void sleepFor (SimulatorClock::duration time);
};

#endif // _ALARMCLOCK_H_