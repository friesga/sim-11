#ifndef _KTF11APAR_H_
#define _KTF11APAR_H_

#include "types.h"
#include "basicregister/basicregister.h"
#include "kd/kdf11_a/ktf11_a/ktf11_apdr.h"

// Definition of the KTF11-A Page Address Register.
//
// The page address register contains the 16-bit page address field (PAF),
// which specifies the starting address of the page as a block number in
// physical memory.
//
class PAR : public BasicRegister
{
public:
    PAR (u16 value, PDR* pdr);
    void operator= (u16 const value) override;

private:
    PDR* pdr_;
};

inline PAR::PAR (u16 value, PDR* pdr)
    :
    BasicRegister (value),
    pdr_ {pdr}
{}

// The PDR's W-bit has to be cleared when the PAR is written.
inline void PAR::operator= (u16 const value)
{
    value_ = value;
    pdr_->clearWriteAccess ();
}


#endif // _KTF11APAR_H_