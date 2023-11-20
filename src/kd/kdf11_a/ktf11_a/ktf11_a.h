#ifndef _KTF11A_H_
#define _KTF11A_H_

#include "qbus/qbus.h"

// The class KTF11_A implements the memory management option for the KDF11-A.
// It implements a subset of the standard PDP-11 Memory Management. There is
// no support for split I/D space and no supervisor mode.
//
// The memory management unit is located between the central processor unit
// and the LSI-II bus address lines. When the memory management unit is
// operating, the normal 16-bit direct byte address is no longer interpreted
// as a direct physical address (PA) but as a virtual address (VA) containing
// information to be used in constructing a new 18- or 22-bit physical
// address. The information contained in the virtual address (VA) is combined
// with relocation information to yield an 18- or 22-bit physical address (PA).
// Using the memory management unit, memory can be dynamically allocated in
// pages, each page composed of from 1 to 128 integral blocks of 32 words. 
// (EK-KDF11-UG-PR2)
//
class KTF11_A
{
public:
	KTF11_A (Qbus* bus);
    CondData<u16> read (u16 address);
	bool writeWord (u16 address, u16 value);
	bool writeByte (u16 address, u8 value);

private:
	Qbus* bus_;
};

#endif _KTF11A_H_