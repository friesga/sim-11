#ifndef _KTF11ASR2_H_
#define _KTF11ASR2_H_

#include "types.h"
#include "registerbase/registerbase.h"

// Definition of KTF11-A Status Register 2
//
// SR2 is loaded with a 16-bit virtual address (VA) during each instruction
// fetch, but is not updated if the instruction fetch fails. SR2 is read-only;
// a write attempt will not modify its contents. SR2 is the virtual address
// program counter. The content of SR2 is frozen whenever one of the abort
// flags (SR0<15:13> is set. 
//
class SR2 : public RegisterBase
{
public:
    SR2 (u16 value);
    void operator= (u16 const value) override;
};

inline SR2::SR2 (u16 value)
    : 
    RegisterBase {value}
{}

// Ignore the write
inline void SR2::operator= (u16 const value)
{}

#endif // _KTF11ASR2_H_