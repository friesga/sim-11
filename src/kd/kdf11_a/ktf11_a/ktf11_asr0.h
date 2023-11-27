#ifndef _KTF11ASR0_H_
#define _KTF11ASR0_H_

#include "types.h"
#include "registerbase/registerbase.h"

// Definition of KTF11-A Status Register 0
class SR0 : public RegisterBase
{
public:
    SR0 (u16 value);
    bool managementEnabled ();

private:
    static const u16 EnableManagementIndex = 0;
	static const u16 EnableManagementMask = bitValue (EnableManagementIndex);

    u16 value_;
};

inline SR0::SR0 (u16 value)
    : 
    RegisterBase {value}
{}

inline bool SR0::managementEnabled ()
{
    return value_ & EnableManagementMask;
}


#endif // _KTF11ASR0_H_