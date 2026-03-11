#ifndef _RACK_UNIT_H_
#define _RACK_UNIT_H_

#include <cstddef>

// Definition of a Rack Unit to avoid confusion with other height definitions.
// 
// A Rack Unit is a standard height of 1.75 inch (44.45 mm).
//
class RackUnit
{
public:
    RackUnit (size_t value);
    operator int () const;

    // Overload operators
    RackUnit& operator= (int value);
    RackUnit& operator++ ();
    RackUnit operator++ (int);
    RackUnit& operator-- ();
    RackUnit operator-- (int);

private:
    size_t value_;
};

inline RackUnit::RackUnit (size_t value)
    :
    value_ {value}
{}

inline RackUnit::operator int () const
{
    return static_cast<int> (value_);
}

inline RackUnit operator""_ru (unsigned long long value)
{
    return RackUnit (value);
}

inline RackUnit& RackUnit::operator= (int value)
{
    value_ = value;
    return *this;
}

inline RackUnit& RackUnit::operator++ ()
{
    value_++;
    return *this;
}

inline RackUnit RackUnit::operator++ (int)
{
    RackUnit temp = *this;
    ++*this;
    return temp;
}

inline RackUnit& RackUnit::operator-- ()
{
    value_--;
    return *this;
}

inline RackUnit RackUnit::operator-- (int)
{
    RackUnit temp = *this;
    --*this;
    return temp;
}

#endif // _RACK_UNIT_H_