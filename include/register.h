#ifndef _REGISTER_H_
#define _REGISTER_H_

#include "types.h"

class Register
{
public:
    virtual void operator= (u16 const value) = 0;
    virtual operator u16 () const = 0;
};


#endif // _REGISTER_H_