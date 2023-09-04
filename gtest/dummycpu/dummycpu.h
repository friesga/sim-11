#ifndef _DUMMYCPU_H_
#define _DUMMYCPU_H_

#include "kd/kd11_na/cpu/cpudata.h"

#include <memory>

using std::unique_ptr;

// Dummy implementation of the CpuData interface to be passed to the
// instruction decoder. It's functions will not be called.
class DummyCpu : public CpuData
{
public:
    using GeneralRegisters = u16[8];

    // Definitions required for the CpuData interface
    GeneralRegisters dummyRegisters;
    u16 dummyPsw;

    DummyCpu ();
	GeneralRegisters& registers () {return dummyRegisters; };
	u16& psw () { return dummyPsw; };
    CondData<u16> fetchWord (u16 address);
	CondData<u8> fetchByte (u16 address);
	bool putWord (u16 address, u16 value);
	bool putByte (u16 address, u8 value);
	void pushWord (u16 value) {};
	bool popWord (u16 *destination) { return false; };

    // Definitions required for the CpuControl interface
    void setTrap (InterruptRequest const *ir) {};
    void cpuReset () {};
    void busReset () {};
    void halt () {};
    void wait () {};
    void start (u16 address) {};
    void proceed () {};
    void inhibitTraceTrap () {};
    u16 registerValue (u8 registerNr) { return 0; };
    void setRegister (u8 registerNr, u16 value) {};
    u16 pswValue () { return 0; };
    void setPSW (u16 value) {};

private:
    // Allocate a memory of 16kW. This allows attempts to read and write
    // from and to non-existing memory addresses.
    static const int memorySize_ {1000};
    unique_ptr<u16[]> memory_;
};

#endif // _DUMMYCPU_H_