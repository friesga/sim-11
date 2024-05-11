#include "alarmwakeupcall.h"
#include "trace/trace.h"

using std::unique_lock;
using std::lock_guard;

AlarmWakeUpCall::AlarmWakeUpCall()
    :
    myId_ {++id_}
{}

void AlarmWakeUpCall::ring (uint64_t currentTime)
{
    lock_guard<std::mutex> lk(wakeUpMutex_);
    awakenedTime_ = currentTime;
    alarmClock_.notify_one ();
}

void AlarmWakeUpCall::waitFor ()
{
    unique_lock<std::mutex> lk (wakeUpMutex_);

    if (awakenedTime_ == 0)
        alarmClock_.wait (lk, [this] {return awakenedTime_ != 0;});
}

size_t AlarmWakeUpCall::id ()
{
    return myId_;
}

size_t AlarmWakeUpCall::id_ {0};