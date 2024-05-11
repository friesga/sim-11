#include "simulatorclock.h"
#include "trace/trace.h"

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

void SimulatorClock::wakeMeAt (time_point timePoint,
    WakeUpCall* wakeUpCall)
{
    lock_guard<mutex> lock {SimulatorClock::clockMutex_};
    wakeUpRequestQueue_.push (WakeUpRequest {timePoint, wakeUpCall});
}

// Note that the ordering of WakeUpRequest objects is based solely on their
// wakeUpTime's. This implies that two WakeUpRequest objects are deemed equal
// while they actually differ (in their WakeUpCall's) and these different
// objects cannot be added to a std::set (which requires it's object are
// unique).
std::strong_ordering SimulatorClock::WakeUpRequest::operator<=> (WakeUpRequest const& rhs) const
{
    return wakeUpTime_ <=> rhs.wakeUpTime_;
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
