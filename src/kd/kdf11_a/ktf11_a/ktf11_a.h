#ifndef _KTF11A_H_
#define _KTF11A_H_

#include "qbus/qbus.h"
#include "kd/include/cpudata.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "apr.h"
#include "ktf11_asr0.h"
#include "registerbase/registerbase.h"

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
class KTF11_A : public AbstractBusDevice
{
public:
	KTF11_A (Qbus* bus, CpuData* cpu);

	// Functions required by the BusDevice interface. The KTF11-A is treated
	// as a bus device as it has registers which have to be accessible on the
	// bus.
	StatusCode read (u16 address, u16 *destination) override;
	StatusCode writeWord (u16 address, u16 value) override;
	bool responsible (u16 address) override;
	void reset () override;

	// The following functions are called from the processor to access the
	// memory and device registers (via the bus).
    CondData<u16> mappedRead (u16 address);
	bool mappedWriteWord (u16 address, u16 value);
	bool mappedWriteByte (u16 address, u8 value);

private:
	// A virtual address is composed of the following fields:
	// - Active Page Field <15:13>,
	// - Block number <12:6>
	// - Displacement in block <5:0>
	//
	static const u16 APFIndex = 13;
	static const u16 APFMask = (u16) (bitField (3) << APFIndex);
	static const u16 blockNrIndex = 6;
	static const u16 blockNrMask = (u16) (bitField (7) << blockNrIndex);
	static const u16 DIBMask {bitField (6)};

	// Definition of the adresses of the KTF11-A
	static const u16 kernelPARBase = 0172340;
	static const u16 kernelPDRBase = 0172300;
	static const u16 userPARBase = 0177640;
	static const u16 userPDRBase = 0177600;
	static const u16 statusRegister0 = 0177572;
	static const u16 statusRegister1 = 0177574;
	static const u16 statusRegister2 = 0177576;
	static const u16 statusRegister3 = 0172516;

	Qbus* bus_;
	CpuData* cpu_;

	// Definition of status registers
	SR0 sr0_ {0};
	RegisterBase sr1_ {0};
	RegisterBase sr2_ {0};
	RegisterBase sr3_ {0};

	// The PSW current memory management mode bits allow the presence of four
	// modes, Kernel, Reserved, Illegal and User, of which only Kernel and
	// User mode are actually used on the KTF11-A. As EK-KDF11-UG-PR2 states
	// that the Reserved and Illegal mode do not cause a halt, we presume the
	// presence of four Active Page Register sets.
	ActivePageRegisterSet activePageRegisterSet_[4];

	u32 physicalAddress (u16 address);
	constexpr u16 activePageField (u16 address);
	constexpr u16 blockNumber (u16 address);
	constexpr u16 displacementInBlock (u16 address);
	u16 pageAddressField (u16 activePageField);
	constexpr u16 memoryManagementMode ();
	RegisterBase* registerPointer (u16 address);
};



#endif // _KTF11A_H_