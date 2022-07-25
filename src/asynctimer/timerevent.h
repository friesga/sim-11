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

    // Handler to be executed at (or after) the specified start time.
    T *handler_;

public:
    TimerEvent ();
    TimerEvent (T *handler, std::chrono::milliseconds period);
    bool operator< (TimerEvent const &te) const;

    // Accessors
    std::chrono::high_resolution_clock::time_point startTime() const;
    T *handler () const;
};

// Default constructor
template <typename T>
TimerEvent<T>::TimerEvent ()
    :
    startTime_ {std::chrono::high_resolution_clock::time_point::max()},
    handler_ {}
{}

// Construct a TimerEvent from the specified function and given period
template <typename T>
TimerEvent<T>::TimerEvent (T *handler,
        std::chrono::milliseconds period)
    :
    startTime_ {std::chrono::high_resolution_clock::now() + period},
    handler_ {handler}
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
T *TimerEvent<T>::handler () const
{
    return handler_;
}

#endif // !_TIMEREVENT_H_