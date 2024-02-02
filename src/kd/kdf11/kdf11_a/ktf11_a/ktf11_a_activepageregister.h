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

    ActivePageRegister ();
    ActivePageRegister (ActivePageRegister const &other);
};

// Definition of a default constructor as the default default constructor
// is deleted because of the definition of the copy constructor.
inline ActivePageRegister::ActivePageRegister ()
{}

// Definition of a a copy constructor to be used for copying the Active Page
// Register in a trace record.
inline ActivePageRegister::ActivePageRegister (ActivePageRegister const &other)
{
    pageAddressRegister_ = other.pageAddressRegister_;
    pageDescripterRegister_ = other.pageDescripterRegister_;
}

#endif // _APR_H_