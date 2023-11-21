#ifndef _KTF11A_H_
#define _KTF11A_H_

#include "qbus/qbus.h"
#include "kd/include/cpudata.h"
#include "apr.h"

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
	KTF11_A (Qbus* bus, CpuData* cpu);
    CondData<u16> read (u16 address);
	bool writeWord (u16 address, u16 value);
	bool writeByte (u16 address, u8 value);

private:
	// A virtual address is composed of the following fields:
	// - Active Page Field <15:13>,
	// - Block number <12:6>
	// - Displacement in block <5:0>
	//
	u16 const APFIndex = 13;
	u16 const APFMask = (u16) (bitField (3) << APFIndex);
	u16 const blockNrIndex = 6;
	u16 const blockNrMask = (u16) (bitField (7) << blockNrIndex);
	u16 const DIBMask {bitField (6)};

	Qbus* bus_;
	CpuData* cpu_;

	// The PSW current memory management mode bits allow the presence of four
	// modes, Kernel, Reserved, Illegal and User, of which only Kernel and
	// User mode are actually used on the KTF11-A. As EK-KDF11-UG-PR2 states
	// that the Reserved and Illegal mode do not cause a halt, we presume the
	// presence of four Active Page Register sets.
	ActivePageRegisterSet activePageRegisterSet_[4];

	u32 constructPhysicalAddress (u16 address);
	constexpr u16 activePageField (u16 address);
	constexpr u16 blockNumber (u16 address);
	constexpr u16 displacementInBlock (u16 address);
	u16 pageAddressField (u16 activePageField);
	constexpr u16 memoryManagementMode ();
};



#endif // _KTF11A_H_