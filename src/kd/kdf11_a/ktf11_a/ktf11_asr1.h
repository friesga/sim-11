#ifndef _KTF11ASR1_H_
#define _KTF11ASR1_H_

#include "types.h"
#include "registerbase/registerbase.h"

// Definition of KTF11-A Status Register 1
//
// SR1 is a read-only register that always reads as zero. 
//
class SR1 : public RegisterBase
{
public:
    SR1 (u16 value);
    void operator= (u16 const value) override;
    operator u16 () override;
};

inline SR1::SR1 (u16 value)
    : 
    RegisterBase {value}
{}

// Ignore the write
inline void SR1::operator= (u16 const value)
{}

inline SR1::operator u16 ()
{
    return 0;
}

#endif // _KTF11ASR1_H_