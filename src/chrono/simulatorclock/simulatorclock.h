#ifndef _SIMULATORCLOCK_H_
#define _SIMULATORCLOCK_H_

#include "../wakeupcall/wakeupcall.h"
#include "../../threadsafecontainers/threadsafeprioqueue.h"

#include <chrono>
#include <chrono>
#include <ratio>
#include <cstdint>
#include <compare>

using std::chrono::time_point;
using std::chrono::duration;
using std::nano;

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

    static time_point currentTime_;
    static ThreadSafePrioQueue<WakeUpRequest> wakeUpRequestQueue_;
};

#endif // !_SIMULATORCLOCK_H_
