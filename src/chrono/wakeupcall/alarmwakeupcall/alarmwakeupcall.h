#ifndef _ALARMWAKEUPCALL_H_
#define _ALARMWAKEUPCALL_H_

#include "../wakeupcall.h"

#include <condition_variable>

using std::condition_variable;
using std::mutex;

class AlarmWakeUpCall : public WakeUpCall
{
public:
    void ring (uint64_t currentTime) override;
    void waitFor () override;

private:
    uint64_t wakeUpTime_ {};
    condition_variable alarmClock_;
    mutex wakeUpMutex_;
};

#endif // _ALARMWAKEUPCALL