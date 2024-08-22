#ifndef _RACK_UNIT_H_
#define _RACK_UNIT_H_

#include <cstddef>

// Definition of a Rack Unit to avoid confusion with other height definitions.
class RackUnit
{
public:
    operator int () const;

    // Overload operators
    RackUnit& operator= (int value);
    RackUnit& operator++ ();
    RackUnit operator++ (int);
    RackUnit& operator-- ();
    RackUnit operator-- (int);

private:
    // The constructor is defined private so it can't be called from outside
    // the class. The literal operator _ru is declared as friend and therefore
    // is able to access the constructor.
    explicit RackUnit (size_t value);

    friend RackUnit operator"" _ru (unsigned long long value);

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

inline RackUnit operator"" _ru (unsigned long long value)
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