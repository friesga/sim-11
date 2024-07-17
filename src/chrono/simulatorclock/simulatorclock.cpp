#include "simulatorclock.h"
#include "trace/trace.h"

void SimulatorClock::reset ()
{
    currentTime_ = SimulatorClock::time_point {SimulatorClock::duration (0)};
}

SimulatorClock::time_point SimulatorClock::now () noexcept
{
    return time_point (currentTime_);
}

void SimulatorClock::forwardClock (duration timeIncrement)
{
    lock_guard<mutex> lock {SimulatorClock::clockMutex_};
    WakeUpRequest nextWakeup;

    currentTime_ += timeIncrement;

    // Check if there are threads to be awakened and if so if it's time
    // to wake them.
    while (!wakeUpRequestQueue_.empty() && 
        currentTime_ >= wakeUpRequestQueue_.top ().wakeUpTime_)
    {
        wakeUpRequestQueue_.fetchTop (nextWakeup);

        // Notify the sleeping thread
        nextWakeup.wakeUpCall_->ring (currentTime_.time_since_epoch ().count ());
    }
}

bool SimulatorClock::wakeMeAt (time_point timePoint,
    WakeUpCall* wakeUpCall)
{
    lock_guard<mutex> lock {SimulatorClock::clockMutex_};
    if (running_)
    {
        wakeUpRequestQueue_.push (WakeUpRequest {timePoint, wakeUpCall});
        return true;
    }

    return false;
}

// Note that the ordering of WakeUpRequest objects is based solely on their
// wakeUpTime's. This implies that two WakeUpRequest objects are deemed equal
// while they actually differ (in their WakeUpCall's) and these different
// objects cannot be added to a std::set (which requires it's object are
// unique).
//
// WakeUpRequest's are stored in a ThreadSafePrioQueue. Such a queue orders
// its elements in ascending priority order. The WakeUpRequest with the
// highest priority is the request with the earliest wake up time. When
// comparing a WakeUpRequest with another (rhs) WakeUpRequest the current
// request is "larger" than the rhs WakeUpRequest if its wakeUpTime_ is
// smaller.
std::strong_ordering SimulatorClock::WakeUpRequest::operator<=> (WakeUpRequest const& rhs) const
{
    return rhs.wakeUpTime_ <=> wakeUpTime_;
}

// When halting the clock all threads waiting to wakened up are awaken
void SimulatorClock::halt ()
{
    lock_guard<mutex> lock {SimulatorClock::clockMutex_};
    WakeUpRequest nextWakeup;

    running_ = false;

    while (!wakeUpRequestQueue_.empty ())
    {
        wakeUpRequestQueue_.fetchTop (nextWakeup);

        // Notify the sleeping thread
        nextWakeup.wakeUpCall_->ring (currentTime_.time_since_epoch ().count ());
    }
}

#ifdef DEBUG
void SimulatorClock::logWakeupRequestQueue (string msg)
{
    string queueContents {msg};
    queueContents += " { ";

    for (auto wakeUpRequestIter = wakeUpRequestQueue_.cbegin ();
        wakeUpRequestIter != wakeUpRequestQueue_.cend ();
        ++wakeUpRequestIter)
    {
        queueContents += to_string (wakeUpRequestIter->wakeUpCall_->id ());
        queueContents += ' ';
    }
    queueContents += "}";
    trace.time (queueContents, currentTime_.time_since_epoch ());
}
#endif

SimulatorClock::time_point SimulatorClock::currentTime_ {};
ThreadSafePrioQueue<SimulatorClock::WakeUpRequest,
    multiset<SimulatorClock::WakeUpRequest>> SimulatorClock::wakeUpRequestQueue_ {};
mutex SimulatorClock::clockMutex_;
bool SimulatorClock::running_ {true};