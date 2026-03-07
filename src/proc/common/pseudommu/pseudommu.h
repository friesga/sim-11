#ifndef _PSEUDOMMU_H_
#define _PSEUDOMMU_H_

#include "proc/include/mmu.h"
#include "bus/include/bus.h"
#include "conddata/conddata.h"
#include "proc/include/cpudata.h"
#include "proc/common/businterface/businterface.h"

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
	PseudoMMU (BusInterface* busInterface, CpuData* cpuData);
    CondData<u16> fetchWord (VirtualAddress address, 
		PSW::Mode mode = PSW::Mode::Default) override;
	CondData<u8> fetchByte (VirtualAddress address, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool putWord (VirtualAddress address, u16 value, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool putByte (VirtualAddress address, u8 value, 
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	CondData<u16> readWithoutTrap (u16 address) override;
	void setVirtualPC (u16 value) override;
	void reset () override;

private:
	BusInterface* busInterface_;
	CpuData* cpuData_;
};

#endif // _PSEUDOMMU_H_