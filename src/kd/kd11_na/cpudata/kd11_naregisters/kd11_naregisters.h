#ifndef _KD11_NAREGISTERS_H_
#define _KD11_NAREGISTERS_H_

#include "kd/include/generalregisters.h"

// This class implements the general register set for the KD11_NA. This
// processor has just R0 - R7.
class KD11_NARegisters : public GeneralRegisters
{
public:
    KD11_NARegisters ();
    u16& operator[] (u16 registerNr) override;
    u16& prevModeContents (u16 registerNr) override;
    u16& contents (u16 registerNr, u16 mode);
    void writePrevMode (u16 registerNr, u16 contents) override;
    operator registerArray() override;

private:
    enum {numRegisters = 8};
    u16 registers_[numRegisters];

    // Disable copy and assignment of KD11_NARegisters objects. This makes
    // it possible to make the KD11_NARegisters object of a processor a
    // public data member.
    KD11_NARegisters (const KD11_NARegisters&) = delete; 
    KD11_NARegisters& operator=(const KD11_NARegisters&) = delete;
};


#endif // _KD11_NAREGISTERS_H_