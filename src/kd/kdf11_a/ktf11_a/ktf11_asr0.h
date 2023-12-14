#ifndef _KTF11ASR0_H_
#define _KTF11ASR0_H_

#include "types.h"
#include "basicregister/basicregister.h"

// Definition of KTF11-A Status Register 0
//
class SR0 : public BasicRegister
{
public:
    SR0 (u16 value);
    bool managementEnabled ();
    bool accessAborted ();

private:
    static const u16 EnableManagementIndex = 0;
	static const u16 EnableManagementMask = bitValue (EnableManagementIndex);
    static const u16 AbortFlagsIndex = 13;
    static const u16 AbortFlagsMask = (u16) (bitField (3) << AbortFlagsIndex);
};

inline SR0::SR0 (u16 value)
    : 
    BasicRegister {value}
{}

inline bool SR0::managementEnabled ()
{
    return value_ & EnableManagementMask;
}

inline bool SR0::accessAborted ()
{
    return value_ & AbortFlagsMask;
}


#endif // _KTF11ASR0_H_