#ifndef _DATAPATHS_H_
#define _DATAPATHS_H_   

#include "bus/include/bus.h"
#include "abstractbusdevice/abstractbusdevice.h"
#include "proc/include/mmu.h"

// The class DataPaths is the providing interface for the CPU to the MMU
// or the bus. The class conforms to the MMU interface as a the MMU must be
// accessable through the DataPaths.
//
class DataPaths 
{
public:
	DataPaths (Bus* bus, MMU* mmu);
	void reset ();
	CondData<u16> fetchWord (VirtualAddress address,
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	CondData<u8> fetchByte (VirtualAddress address,
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	bool putWord (VirtualAddress address, u16 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	bool putByte (VirtualAddress address, u8 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default);

	void setVirtualPC (u16 value);
	CondData<u16> readWithoutTrap (u16 address);

private:
    Bus* bus_;
    MMU* mmu_;
};


#endif // _DATAPATHS_H_