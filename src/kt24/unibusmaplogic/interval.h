#ifndef _INTERVAL_H_
#define _INTERVAL_H_

// This class defines an interval of values of type T. The interval contains
// the values starting at the given low up (not including) the given high
// value. This definition corresponds to the definition of C++ ranges.
template<typename T>
class Interval
{
public:
    Interval (T low, T high);
    bool contains (T value) const;

private:
    T low_;
    T high_;
};

template <typename T>
Interval<T>::Interval (T low, T high)
    :
    low_ {low},
    high_ {high}
{
}

template <typename T>
bool Interval<T>::contains (T value) const
{
    return value >= low_ && value < high_;
}

#endif // _INTERVAL_H_