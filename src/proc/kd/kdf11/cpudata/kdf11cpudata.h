#ifndef _KDF11ACPUDATA_H_
#define _KDF11ACPUDATA_H_

#include "proc/common/cpudata/basecpudata/basecpudata.h"
#include "kdf11registers/kdf11registers.h"
#include "kdf11_psw.h"
#include "proc/common/cpudata/pswbusdevice/pswbusdevice.h"

//
// The class KDF11CpuData provides the KDF11-spcific version of the CpuData,
// the BusDevice interface for bus access to the PSW and implements the
// stack overflow functions required by the CpuData interface.
//
class KDF11CpuData : public BaseCpuData<KDF11Registers, KDF11_PSW>,
    public PSWBusDevice<KDF11CpuData>
{
public:
	// Functions required by the CpuData interface and not implemented by
	// BaseCpuData
	bool stackOverflow () override;

    // Functions used by the PSWBusDevice class
	BaseCpuData& cpuData () noexcept { return *this; }
	BaseCpuData const& cpuData () const noexcept { return *this; }

private:
	enum {stackLimit = 0400};

	constexpr bool inKernelMode ();
};

// Check if a stack overflow has occurred, i.e. the kernel stack pointer has
// been decremented below the stack limit.
// On a double bus error a new stack will be set up at locations 2 and 0. This
// should not result in a stack overflow trap.
inline bool KDF11CpuData::stackOverflow ()
{
    return inKernelMode () && 
        registers_ [6] > 0 && registers_ [6] < stackLimit;
}

 constexpr bool KDF11CpuData::inKernelMode ()
 {
	 return psw_.currentMode () == PSW::Mode::Kernel;
 }

#endif // _KDF11ACPUDATA_H_