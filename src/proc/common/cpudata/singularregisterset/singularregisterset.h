#ifndef _SINGULARREGISTERSET_H_
#define _SINGULARREGISTERSET_H_

#include "proc/kd/include/generalregisters.h"

// This class implements a singular register set, i.e. it contains just a 
// single copy of R0-R7. This set is used by the KD11-NA and KA11 processors.
//
class SingularRegisterSet : public GeneralRegisters
{
public:
    SingularRegisterSet (u16 const &psw);
    u16& operator[] (u16 registerNr) override;
    u16& prevModeContents (u16 registerNr) override;
    u16& contents (u16 registerNr, u16 mode);
    void writePrevMode (u16 registerNr, u16 contents) override;
    operator registerArray() override;

private:
    enum {numRegisters = 8};
    u16 registers_[numRegisters];

    // Disable copy and assignment of SingularRegisterSet objects. This makes
    // it possible to make the SingularRegisterSet object of a processor a
    // public data member.
    SingularRegisterSet (const SingularRegisterSet&) = delete; 
    SingularRegisterSet& operator=(const SingularRegisterSet&) = delete;
};

#endif // _SINGULARREGISTERSET_H_