#ifndef _BASICREGISTER_H_
#define _BASICREGISTER_H_

#include "register.h"

// Definition of a basic register that can be assigned to and read from
//
class BasicRegister : public Register
{
public:
    BasicRegister (u16 const value);
    void operator= (u16 const value) override;
    operator u16 () const override;

protected:
    u16 value_;
};

inline BasicRegister::BasicRegister (u16 const value)
    :
    value_ {value}
{}

inline void BasicRegister::operator= (u16 const value)
{
    value_ = value;
}

inline BasicRegister::operator u16 () const
{
    return value_;
}


#endif // _BASICREGISTER_H_