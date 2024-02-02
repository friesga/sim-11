#ifndef _KDF11ACPUDATA_H_
#define _KDF11ACPUDATA_H_

#include "kd/common/kdcpudata/kdcpudata.h"
#include "kdf11_aregisters/kdf11_aregisters.h"
#include "kdf11_a_psw.h"
#include "abstractbusdevice/abstractbusdevice.h"

//
// The class KDF11_ACpuData implements the CpuData interface for the KDF11-A
// plus the BusDevice interface for bus access to the PSW.
//
class KDF11_ACpuData : public KDCpuData<KDF11_ARegisters, KDF11_A_PSW>,
	public AbstractBusDevice
{
public:
	// Functions required by the CpuData interface and not implemented by
	// KDCpuData
	bool stackOverflow () override;

	// Functions required by the BusDevice interface and not implemented by
	// AbstractBusDevice.
	StatusCode read (BusAddress address, u16 *destination) override;
	StatusCode writeWord (BusAddress address, u16 value) override;
	bool responsible (BusAddress address) override;
	void reset () override;

private:
	enum {stackLimit = 0400};
	enum { PSWAddress = 0177776 };

	constexpr bool inKernelMode ();
};

// Check if a stack overflow has occurred, i.e. the kernel stack pointer has
// been decremented below the stack limit.
// On a double bus error a new stack will be set up at locations 2 and 0. This
// should not result in a stack overflow trap.
inline bool KDF11_ACpuData::stackOverflow ()
{
    return inKernelMode () && 
        registers_ [6] > 0 && registers_ [6] < stackLimit;
}

 constexpr bool KDF11_ACpuData::inKernelMode ()
 {
	 return psw_.currentMode () == PSW::Mode::Kernel;
 }

#endif // _KDF11ACPUDATA_H_