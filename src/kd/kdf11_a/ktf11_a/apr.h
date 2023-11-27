#ifndef _APR_H_
#define _APR_H_

#include "registerbase/registerbase.h"

// This struct defines the Active Page Register (APR). The register
// comprises two 16-bit registers: the Page Address Register (PAR) and the
// Page Description Register (PDR).
// 
// The Page Address Register contains the Page Address Field (PAF).
//
struct ActivePageRegister
{
    RegisterBase PageAddressRegister_ {0};
    RegisterBase PageDescripterRegister_ {0};

    u16 AccessControlField ();
};

// This struct defines a set of Active Page Registers.
struct ActivePageRegisterSet
{
    ActivePageRegister activePageRegister_[8];
};



#endif // _APR_H_