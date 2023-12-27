#ifndef _DUMMYMMU_H_
#define _DUMMYMMU_H_

#include "kd/include/mmu.h"
#include "types.h"

#include <memory>

using std::unique_ptr;

class DummyMMU : public MMU
{
public:
	DummyMMU ();
	void reset () {};
    CondData<u16> fetchWord (u16 address, 
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	CondData<u8> fetchByte (u16 address,
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	bool putWord (u16 address, u16 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	bool putByte (u16 address, u8 value,
		PSW::Mode memMgmtMode = PSW::Mode::Default);
	bool pushWord (u16 value) { return true; };
	bool popWord (u16 *destination) { return false; };
	CondData<u16> readWithoutTrap (u16 address) { return fetchWord (address); };
	void setVirtualPC (u16 value) {};

private:
    // Allocate a memory of 16kW. This allows attempts to read and write
    // from and to non-existing memory addresses.
    static const int memorySize_ {1000};
    unique_ptr<u16[]> memory_;

	CondData<u16> mappedRead (u16 address) { return fetchWord (address); };
	bool mappedWriteWord (u16 address, u16 value) { return putWord (address, value); };
	bool mappedWriteByte (u16 address, u8 value) { return putByte (address, value); };
};


#endif // _DUMMYMMU_H_