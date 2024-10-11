#include "asynctimer.h"

// Constructor
AsyncTimer::AsyncTimer ()
    :
    stop_ {false}
{
    handleThread_ = std::thread (&AsyncTimer::handle, this);
}

AsyncTimer::~AsyncTimer ()
{
    stop_ = true;
    queueEvent_.notify_one ();
    handleThread_.join ();
}


void AsyncTimer::start (std::function<void (void)> func,
    std::chrono::milliseconds timeFromNow, void* id)
{
    // Create TimerEvent and push it in the timer queue
    timerQueue_.push (TimerEvent (func, timeFromNow, id));

    // Notify the handler thread a TimerEvent has been pushed
    // ToDo: Lock the mutex??
    queueEvent_.notify_one ();
}

// Cancel all TimerEvents with the specified id
void AsyncTimer::cancel (void* id)
{
    // Special care has to be taken with the handling of the iterator. Erasing
    // an element in the queue might invalidate the current iterator.
    // Therefore determine the next iterator before the element the iterator
    // points at is erased.
    auto iter = timerQueue_.cbegin ();
    while (iter != timerQueue_.cend ())
    {
        auto nextIter = iter;
        ++nextIter;

        if (iter->id () == id)
            timerQueue_.erase (*iter);

        iter = nextIter;

        // Notify the handler thread a TimerEvent has been deleted
        queueEvent_.notify_one ();
    }
}

// Handle the TimerEvents
void AsyncTimer::handle ()
{
    std::unique_lock<std::mutex> lock (mutex_);

    while (!stop_)
    {
        // Determine the time to wait for the occurence of a TimerEvent. If the
        // queue is empty we wait till we are notified of a queue change event.
        std::chrono::high_resolution_clock::time_point tp = timerQueue_.empty () ?
            std::chrono::high_resolution_clock::time_point::max () :
            timerQueue_.top ().startTime ();

        // Wait until with a condition variable for that time event
        if (queueEvent_.wait_until (lock, tp) == std::cv_status::timeout)
        {
            // Awakened because of the specified time_point. The TimerEvent
            // we were waiting for should still be at the top of the queue
            // as we haven't been notified of a change in the queue
            TimerEvent te;
            timerQueue_.fetchTop (te);

            // Call the function specified in the TimerEvent
            (te.function ()) ();
        }
        else
        {
            // Awakened because a TimerEvent has been pushed or removed
            // from the queue or a spurious wakeup occurred. Continue with
            // a wait_until for the top of the queue
            continue;
        }
    }
}

// Return the running status of the timer with the specified id. The timer
// is running if it is in timer queue
bool AsyncTimer::isRunning (void* id)
{
    std::unique_lock<std::mutex> lock (mutex_);

    for (auto iter = timerQueue_.cbegin (); iter != timerQueue_.cend (); ++iter)
    {
        if (iter->id () == id)
            return true;
    }

    return false;
}
