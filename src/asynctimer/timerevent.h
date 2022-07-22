#ifndef _TIMEREVENT_H_
#define _TIMEREVENT_H_

#include <chrono>
#include <functional>

// A TimerEvent specified a function to execute at the specified time point
class TimerEvent
{
    // The time point to execute the function
    std::chrono::high_resolution_clock::time_point startTime_;

    // Function to be executed at (or after) the specified start time.
    std::function<void(void)> function_;

    // Pointer identifying the current TimerEvent
    void *id_;

public:
    TimerEvent ();
    TimerEvent (std::function<void(void)> func, 
        std::chrono::milliseconds period, void *id);
    bool operator< (TimerEvent const &te) const;

    // Accessors
    std::chrono::high_resolution_clock::time_point startTime() const;
    std::function<void(void)> &function ();
    void *id() const;
};

// Default constructor
TimerEvent::TimerEvent ()
    :
    startTime_ {std::chrono::high_resolution_clock::time_point::max()},
    function_ {nullptr}
{}

// Construct a TimerEvent from the specified function and given period
TimerEvent::TimerEvent (std::function<void(void)> func, 
    std::chrono::milliseconds period, void *id)
    :
    startTime_ {std::chrono::high_resolution_clock::now() + period},
    function_ {func},
    id_ {id}
{}

// TimerEvents are ordered in increasing starttime; the earliest time point
// is at the top of the queue
bool TimerEvent::operator< (TimerEvent const &te) const
{
    return te.startTime_ < startTime_;
}

// Accessors
std::chrono::high_resolution_clock::time_point TimerEvent::startTime() const
{
    return startTime_;
}

std::function<void(void)> &TimerEvent::function ()
{
    return function_;
}

void *TimerEvent::id () const
{
    return id_;
}

#endif // !_TIMEREVENT_H_