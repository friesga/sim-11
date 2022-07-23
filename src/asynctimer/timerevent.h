#ifndef _TIMEREVENT_H_
#define _TIMEREVENT_H_

#include <chrono>
#include <functional>

// A TimerEvent specified a function to execute at the specified time point
template <typename T>
class TimerEvent
{
    // The time point to execute the function
    std::chrono::high_resolution_clock::time_point startTime_;

    // Function to be executed at (or after) the specified start time.
    std::function<void(T)> function_;

    // Pointer identifying the current TimerEvent
    T id_;

public:
    TimerEvent ();
    TimerEvent (std::function<void(T)> func, T id,
        std::chrono::milliseconds period);
    bool operator< (TimerEvent const &te) const;

    // Accessors
    std::chrono::high_resolution_clock::time_point startTime() const;
    std::function<void(T)> &function ();
    T id() const;
};

// Default constructor
template <typename T>
TimerEvent<T>::TimerEvent ()
    :
    startTime_ {std::chrono::high_resolution_clock::time_point::max()},
    function_ {nullptr},
    id_ {}
{}

// Construct a TimerEvent from the specified function and given period
template <typename T>
TimerEvent<T>::TimerEvent (std::function<void(T)> func, T id,
        std::chrono::milliseconds period)
    :
    startTime_ {std::chrono::high_resolution_clock::now() + period},
    function_ {func},
    id_ {id}
{}

// TimerEvents are ordered in increasing starttime; the earliest time point
// is at the top of the queue
template <typename T>
bool TimerEvent<T>::operator< (TimerEvent const &te) const
{
    return te.startTime_ < startTime_;
}

// Accessors
template <typename T>
std::chrono::high_resolution_clock::time_point TimerEvent<T>::startTime() const
{
    return startTime_;
}

template <typename T>
std::function<void(T)> &TimerEvent<T>::function ()
{
    return function_;
}

template <typename T>
T TimerEvent<T>::id () const
{
    return id_;
}

#endif // !_TIMEREVENT_H_