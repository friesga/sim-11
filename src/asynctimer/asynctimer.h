#ifndef _ASYNCTIMER_H_
#define _ASYNCTIMER_H_

#include "threadsafecontainers/threadsafeprioqueue.h"
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
class AsyncTimer
{
    using TimerQueue = ThreadSafePrioQueue<TimerEvent>;
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
    void start (std::function<void(void)> func, 
        std::chrono::milliseconds fromNow, void *ptr);
    void cancel (void *id);
    void handle ();
    bool isRunning (void *id);
};


#endif // !_ASYNCTIMER_H_
