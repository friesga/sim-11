#ifndef _PSEUDOMMU_H_
#define _PSEUDOMMU_H_

#include "kd/include/mmu.h"
#include "qbus/qbus.h"
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
	PseudoMMU (Qbus* bus, CpuData* cpuData);
    CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;
	bool pushWord (u16 value) override;
	bool popWord (u16 *destination) override;

	CondData<u16> mappedRead (u16 address) override;
	bool mappedWriteWord (u16 address, u16 value) override;
	bool mappedWriteByte (u16 address, u8 value) override;

	void setVirtualPC (u16 value) override;

private:
	Qbus* bus_;
	CpuData* cpuData_;
};

#endif // _PSEUDOMMU_H_