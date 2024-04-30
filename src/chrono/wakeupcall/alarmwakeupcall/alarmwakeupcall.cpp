#include "alarmwakeupcall.h"

using std::unique_lock;

void AlarmWakeUpCall::ring (uint64_t currentTime)
{
    wakeUpTime_ = currentTime;
    alarmClock_.notify_one ();
}

void AlarmWakeUpCall::waitFor ()
{
    unique_lock<std::mutex> lk (wakeUpMutex_);
    alarmClock_.wait (lk, [this] {return wakeUpTime_ != 0;});
}