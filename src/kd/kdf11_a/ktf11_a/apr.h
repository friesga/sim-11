#ifndef _APR_H_
#define _APR_H_

#include "basicregister/basicregister.h"
#include "ktf11_apdr.h"
#include "ktf11_apar.h"

// This struct defines the Active Page Register (APR). The register
// comprises two 16-bit registers: the Page Address Register (PAR) and the
// Page Description Register (PDR).
// 
// The Page Address Register contains the Page Address Field (PAF).
//
struct ActivePageRegister
{
    PAR pageAddressRegister_ {0, &pageDescripterRegister_};
    PDR pageDescripterRegister_ {0};
};

// This struct defines a set of Active Page Registers.
struct ActivePageRegisterSet
{
    ActivePageRegister activePageRegister_[8];
};

#endif // _APR_H_