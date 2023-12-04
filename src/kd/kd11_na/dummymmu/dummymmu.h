#include "kd/include/mmu.h"
#include "qbus/qbus.h"
#include "conddata/conddata.h"
#include "kd/include/cpudata.h"

// As the KD11-NA has no MMU, this class handles virtual addresses as
// physicals addresses.
class DummyMMU : public MMU
{
public:
	DummyMMU (Qbus* bus, CpuData* cppuData);
    CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;
	bool pushWord (u16 value) override;
	bool popWord (u16 *destination) override;

private:
	Qbus* bus_;
	CpuData* cpuData_;
};