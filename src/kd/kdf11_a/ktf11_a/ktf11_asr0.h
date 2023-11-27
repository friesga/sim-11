#ifndef _KTF11ASR0_H_
#define _KTF11ASR0_H_

#include "types.h"

// Definition of KTF11-A Status Register 0
class SR0
{
public:
    SR0 (u16 value);
    void operator= (u16 const value);
    operator u16 ();
    bool managementEnabled ();

private:
    static const u16 EnableManagementIndex = 0;
	static const u16 EnableManagementMask = bitValue (EnableManagementIndex);

    u16 value_;
};

inline SR0::SR0 (u16 value)
    : 
    value_ {value}
{}

inline void SR0::operator= (u16 const value)
{
    value_ = value;
}

inline SR0::operator u16 ()
{
    return value_;
}

inline bool SR0::managementEnabled ()
{
    return value_ & EnableManagementMask;
}


#endif // _KTF11ASR0_H_