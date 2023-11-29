#ifndef _REGISTERBASE_H_
#define _REGISTERBASE_H_

#include "register.h"

// Provide implementations for operator=() and operator u16 ().
//
// Note that the assignment operator in a derived class is not inherited from
// the base class and has to be defined explicitly.
//
class RegisterBase : public Register
{
public:
    RegisterBase (u16 const value);
    void operator= (u16 const value) override;
    operator u16 () override;

protected:
    u16 value_;
};

inline RegisterBase::RegisterBase (u16 const value)
    :
    value_ {value}
{}

inline void RegisterBase::operator= (u16 const value)
{
    value_ = value;
}

inline RegisterBase::operator u16 ()
{
    return value_;
}


#endif // _REGISTERBASE_H_