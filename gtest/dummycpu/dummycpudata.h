#ifndef _DUMMYCPUDATA_H_
#define _DUMMYCPUDATA_H_

#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpudata/kd11_naregisters/kd11_naregisters.h"

class DummyCpuData : public CpuData
{
public:
    GeneralRegisters& registers () {return dummyRegisters; };
	u16 psw () { return dummyPsw; }
    void setPSW (u16 value) {};
    void loadPSW (u16 value) {};
    void setCC (ConditionCodes conditionCodes) {};
    bool stackOverflow () {return false;};

    TrapCondition trap () { return TrapCondition::None; };
    void setTrap (TrapCondition trap, TrapRecordType cause) {};
    void clearTrap () {};
    u16 trapVector () { return 0; };
    u16 trapVector (TrapCondition trap) { return 0; };

private:
    // Use the KD11_NARegisters as that is the most simple GeneralRegisters
    // implementation.
    KD11_NARegisters dummyRegisters;
    u16 dummyPsw;
};


#endif // _DUMMYCPUDATA_H_