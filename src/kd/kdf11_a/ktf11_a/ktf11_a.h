#ifndef _KTF11A_H_
#define _KTF11A_H_

#include "qbus/qbus.h"
#include "kd/include/cpudata.h"
#include "kd/include/mmu.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "ktf11_a_activepageregisterset.h"
#include "ktf11_asr0.h"
#include "ktf11_asr3.h"
#include "basicregister/basicregister.h"
#include "basicregister/readonlyregister.h"

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
class KTF11_A : public MMU, public AbstractBusDevice
{
public:
	KTF11_A (Qbus* bus, CpuData* cpuData);

	// Functions reuiqred by the MMU interface
	CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;
	bool pushWord (u16 value) override;
	bool popWord (u16 *destination) override;

	// The following functions use the given adress as a physical address
	// (in case memory management is disabled) to access memory or map the
	// virtual address to a physical address when memory management is 
	// enabled.
    CondData<u16> mappedRead (u16 address) override;
	bool mappedWriteWord (u16 address, u16 value) override;
	bool mappedWriteByte (u16 address, u8 value) override;
	void setVirtualPC (u16 value) override;

	CondData<u16> readWithoutTrap (u16 address) override;

	// Functions required by the BusDevice interface and not implemented by
	// AbstractBusDevice.
	StatusCode read (u16 address, u16 *destination) override;
	StatusCode writeWord (u16 address, u16 value) override;
	bool responsible (u16 address) override;
	void reset () override;

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
	CpuData* cpuData_;

	// Definition of status registers
	//
	// SR0 contains abort error flags, memory management enable, and other
	// information essential for an operating system to recover from an abort
	// or to service a memory management trap. 
	// 
	// SR1 is a read-only register that always reads as zero.
	// 
	// SR2 is loaded with a 16-bit virtual address (VA) during each instruction
	// fetch, but is not updated if the instruction fetch fails. SR2 is
	// read-only; a write attempt will not modify its contents. SR2 is the
	// virtual address program counter. The content of SR2 is frozen whenever
	// one of the abort flags (SR0<15:13>) is set.
	//
	// SR3 bit <4> enables or disables the memory management 22-bit mapping. 
	// SR3 bit <5> is a read/write bit that has no effect on KDF11-BA operation.
	//
	SR0 sr0_ {0};
	BasicRegister const sr1_ {0};
	BasicRegister sr2_ {0};
	SR3 sr3_ {0};

	// The ReadonlyRegisters form a kind of facade or decorator which
	// removes the possibility to write to a register from the specified
	// register.
	ReadOnlyRegister readOnlySr1_ {sr1_};
	ReadOnlyRegister readOnlySr2_ {sr2_};

	// The PSW current memory management mode bits allow the presence of four
	// modes, Kernel, Reserved, Illegal and User, of which only Kernel and
	// User mode are actually used on the KTF11-A. As EK-KDF11-UG-PR2 states
	// that the Reserved and Illegal mode do not cause a halt, we presume the
	// presence of four Active Page Register sets.
	ActivePageRegisterSet activePageRegisterSet_[4];

	u32 physicalAddress (u16 address);
	u32 physicalAddress (u16 address, ActivePageRegister* apr);
	ActivePageRegister *activePageRegister (u16 address);
	constexpr u16 activePageField (u16 address);
	constexpr u16 blockNumber (u16 address);
	constexpr u16 displacementInBlock (u16 address);
	u16 pageAddressField (u16 activePageField);
	constexpr u16 memoryManagementMode ();
	Register* registerPointer (u16 address);
	bool readAllowed (PDR const & pdr);
	CondData<u16> readPhysical (u16 address);
};



#endif // _KTF11A_H_