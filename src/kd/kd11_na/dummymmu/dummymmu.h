#include "kd/include/mmu.h"
#include "qbus/qbus.h"
#include "conddata/conddata.h"
#include "kd/include/cpudata.h"

// As the KD11-NA has no MMU, this class handles virtual addresses as
// physicals addresses.
class DummyMMU : public MMU
{
public:
	void reset ();
	DummyMMU (Qbus* bus, CpuData* cpuData);
    CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;
	bool pushWord (u16 value) override;
	bool popWord (u16 *destination) override;

	CondData<u16> mappedRead (u16 address) override;
	bool mappedWriteWord (u16 address, u16 value) override;
	bool mappedWriteByte (u16 address, u8 value) override;

	void setSR2 (u16 value) override;

private:
	Qbus* bus_;
	CpuData* cpuData_;
};