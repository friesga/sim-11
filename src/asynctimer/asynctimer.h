#ifndef _ASYNCTIMER_H_
#define _ASYNCTIMER_H_

#include "threadsafeprioqueue/threadsafeprioqueue.h"
#include "timerevent.h"

#include <chrono>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>

#include <iostream>

// A class keeping the queue with TimerEvents. Processing of the
// TimerEvents is performed in a seperate thread. The timer queue is 
// maintained as a thread safe priority queue.
template <typename T>
class AsyncTimer
{
    using TimerQueue = ThreadSafePrioQueue<TimerEvent<T>>;
    TimerQueue timerQueue_;

    std::mutex mutex_;
    std::condition_variable queueEvent_;

    // Indicate the handler has to stop
    bool stop_;

    // Handler thread
    std::thread handleThread_;

public:
    AsyncTimer ();
    ~AsyncTimer ();
    void start (T *handler, std::chrono::milliseconds fromNow);
    void cancel (T *handler);
    void handle ();
    bool isRunning (T *handler);
};

// Constructor
template <typename T>
AsyncTimer<T>::AsyncTimer ()
    :
    stop_ {false}
{
    handleThread_ = std::thread (&AsyncTimer::handle, this);
}

template <typename T>
AsyncTimer<T>::~AsyncTimer ()
{
    stop_ = true;
    queueEvent_.notify_one ();
    handleThread_.join ();
}

template <typename T>
void AsyncTimer<T>::start (T *handler, std::chrono::milliseconds timeFromNow)
{
    // Create TimerEvent and push it in the timer queue
    timerQueue_.push (TimerEvent (handler, timeFromNow));

    // Notify the handler thread a TimerEvent has been pushed
    // ToDo: Lock the mutex??
    queueEvent_.notify_one ();
}

// Cancel all TimerEvents with the specified id
template <typename T>
void AsyncTimer<T>::cancel (T *handler)
{
    // Special care has to be taken with the handling of the iterator. Erasing
    // an element in the queue might invalidate the current iterator.
    // Therefore determine the next iterator before the element the iterator
    // points at is erased.
    auto iter = timerQueue_.cbegin();
    while (iter != timerQueue_.cend())
    {
        auto nextIter = iter;
        ++nextIter;

        if (iter->handler() == handler)
          timerQueue_.erase (*iter);
        
        iter = nextIter;

        // Notify the handler thread a TimerEvent has been deleted
        queueEvent_.notify_one ();
    }
}

// Handle the TimerEvents
template <typename T>
void AsyncTimer<T>::handle ()
{
    std::unique_lock<std::mutex> lock (mutex_);

    while (!stop_)
    {
        // Determine the time to wait for the occurence of a TimerEvent. If the
        // queue is empty we wait till we are notified of a queue change event.
        std::chrono::high_resolution_clock::time_point tp = timerQueue_.empty() ? 
            std::chrono::high_resolution_clock::time_point::max() :
            timerQueue_.top().startTime();

        // Wait until with a condition variable for that time event
        if (queueEvent_.wait_until (lock, tp) == std::cv_status::timeout)
        {
            // Awakened because of the specified time_point. The TimerEvent
            // we were waiting for should still be at the top of the queue
            // as we haven't been notified of a change in the queue
            TimerEvent<T> te;
            timerQueue_.fetchTop(te);

            // Call the handler specified in the TimerEvent
            (*te.handler()) ();
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
template <typename T>
bool AsyncTimer<T>::isRunning (T *handler)
{
    std::unique_lock<std::mutex> lock (mutex_);

    for (auto iter = timerQueue_.cbegin(); iter != timerQueue_.cend(); ++iter)
    {
        if (iter->handler () == handler)
            return true;
    }

    return false;
}

#endif // !_ASYNCTIMER_H_
