#ifndef _DATAPATHS_H_
#define _DATAPATHS_H_   

#include "bus/include/bus.h"
#include "proc/include/mmu.h"

// The class DataPaths is the providing interface for the CPU to the MMU
// or the bus. The class conforms to the MMU interface as a the MMU must be
// accesable through the DataPaths.
//
class DataPaths : public MMU
{
public:
	DataPaths (Bus* bus, MMU* mmu);
	void reset () override;
	CondData<u16> fetchWord (VirtualAddress address,
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	CondData<u8> fetchByte (VirtualAddress address,
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool putWord (VirtualAddress address, u16 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool putByte (VirtualAddress address, u8 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default) override;
	bool pushWord (u16 value) override;
	bool popWord (u16* destination) override;

	void setVirtualPC (u16 value) override;
	CondData<u16> readWithoutTrap (u16 address) override;

private:
    Bus* bus_;
    MMU* mmu_;
};


#endif // _DATAPATHS_H_