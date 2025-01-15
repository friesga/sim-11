#ifndef _PSEUDOMMU_H_
#define _PSEUDOMMU_H_

#include "kd/include/mmu.h"
#include "bus/bus.h"
#include "conddata/conddata.h"
#include "kd/include/cpudata.h"

//
// Pseudo: being apparently rather than actually as stated 
// (https://www.merriam-webster.com/dictionary/pseudo)
// 
// As the KD11-NA has no MMU, this class handles virtual addresses as
// physical addresses.
//
class PseudoMMU : public MMU
{
public:
	void reset ();
	PseudoMMU (Bus* bus, CpuData* cpuData);
    CondData<u16> fetchWord (VirtualAddress address, 
		PSW::Mode mode = PSW::Mode::Default) override;
	CondData<u8> fetchByte (VirtualAddress address, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool putWord (VirtualAddress address, u16 value, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool putByte (VirtualAddress address, u8 value, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool pushWord (u16 value) override;
	bool popWord (u16 *destination) override;
	CondData<u16> readWithoutTrap (u16 address) override;
	void setVirtualPC (u16 value) override;

private:
	Bus* bus_;
	CpuData* cpuData_;

	CondData<u16> mappedRead (u16 address);
	bool mappedWriteWord (u16 address, u16 value);
	bool mappedWriteByte (u16 address, u8 value);
};

#endif // _PSEUDOMMU_H_