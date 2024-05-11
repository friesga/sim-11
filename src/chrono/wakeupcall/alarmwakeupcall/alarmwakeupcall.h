#ifndef _ALARMWAKEUPCALL_H_
#define _ALARMWAKEUPCALL_H_

#include "../wakeupcall.h"
#include "chrono/simulatorclock/simulatorclock.h"

#include <condition_variable>

using std::condition_variable;
using std::mutex;

class AlarmWakeUpCall : public WakeUpCall
{
public:
    AlarmWakeUpCall ();
    void ring (uint64_t currentTime) override;
    void waitFor () override;
    size_t id ();

private:
    uint64_t awakenedTime_ {};
    condition_variable alarmClock_;
    mutex wakeUpMutex_;
    size_t myId_;

    static size_t id_;
};

#endif // _ALARMWAKEUPCALL