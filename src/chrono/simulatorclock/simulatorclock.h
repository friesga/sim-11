#ifndef _SIMULATORCLOCK_H_
#define _SIMULATORCLOCK_H_

#include "../wakeupcall/wakeupcall.h"
#include "../../threadsafecontainers/threadsafeprioqueue.h"

#include <chrono>
#include <chrono>
#include <ratio>
#include <cstdint>
#include <compare>
#include <string>

using std::chrono::time_point;
using std::chrono::duration;
using std::nano;
using std::string;
using std::multiset;

// This class keeps track of the simulator internal clock and schedules
// thread wakeups according to this clock.
//
class SimulatorClock
{
public:
    using duration = std::chrono::duration<uint64_t, nano>;
    using rep = duration::rep;
    using period = duration::period;
    using time_point = std::chrono::time_point<SimulatorClock>;

    static const bool is_steady = true;

    static time_point now () noexcept;
    static void forwardClock (duration timeIncrement);
    static void wakeMeAt (time_point timePoint, WakeUpCall* wakeUpCall);

private:
    struct WakeUpRequest
    {
        std::strong_ordering operator<=> (WakeUpRequest const& rhs) const;

        time_point wakeUpTime_ {};
        WakeUpCall* wakeUpCall_ {nullptr};
    };
  
#ifdef DEBUG
    static void logWakeupRequestQueue (string msg);
#endif

    static time_point currentTime_;

    // Use a multiset as the basis for the ThreadSafePrioQueue to allow
    // "duplicate" WakeUpRequest objects to be added to the queue.
    static ThreadSafePrioQueue<WakeUpRequest, multiset<WakeUpRequest>> wakeUpRequestQueue_;
    static mutex clockMutex_;
};

#endif // !_SIMULATORCLOCK_H_
