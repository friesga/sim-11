#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "types.h"

template <typename Derived>
class Register
{
public:
    Register<Derived>& operator= (const u16& value);
    operator u16 ();
};


template <typename Derived>
Register<Derived>& Register<Derived>::operator= (const u16& value)
{
    static_cast<Derived*> (this)->assignmentOperatorImpl (value);
    return *this;
}

template <typename Derived>
Register<Derived>::operator u16 ()
{
    return static_cast<Derived*> (this)->operatoru16Impl ();
}

#endif // _REGISTER_H_