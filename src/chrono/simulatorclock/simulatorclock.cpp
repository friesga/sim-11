#include "simulatorclock.h"

SimulatorClock::time_point SimulatorClock::now () noexcept
{
    return time_point (currentTime_);
}

void SimulatorClock::forwardClock (duration timeIncrement)
{
    WakeUpRequest nextWakeup;

    currentTime_ += timeIncrement;

    // Check if there is a thread to be awakened and if so if it's time
    // to wake it.
    if (!wakeUpRequestQueue_.empty() && 
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
    wakeUpRequestQueue_.push (WakeUpRequest {timePoint, wakeUpCall});
}

std::strong_ordering SimulatorClock::WakeUpRequest::operator<=> (WakeUpRequest const& rhs) const
{
    return wakeUpTime_ <=> rhs.wakeUpTime_;
}

SimulatorClock::time_point SimulatorClock::currentTime_ {};
ThreadSafePrioQueue<SimulatorClock::WakeUpRequest> SimulatorClock::wakeUpRequestQueue_ {};
