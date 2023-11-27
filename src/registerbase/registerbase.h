#ifndef _REGISTERBASE_H_
#define _REGISTERBASE_H_

#include "register.h"

// Provide implementations for operator=() and operator u16 ().
//
// Note that the assignment operator in a derived class is not inherited from
// the base class and has to be defined explicitly.
//
class RegisterBase : public Register<RegisterBase>
{
public:
    using Register<RegisterBase>::operator=;
    void assignmentOperatorImpl (u16 const value);
    u16 operatoru16Impl ();

protected:
    u16 value_;
};

inline void RegisterBase::assignmentOperatorImpl (u16 const value)
{
    value_ = value;
}

inline u16 RegisterBase::operatoru16Impl ()
{
    return value_;
}


#endif // _REGISTERBASE_H_