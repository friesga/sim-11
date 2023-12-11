#ifndef _KDF11ACPUDATA_H_
#define _KDF11ACPUDATA_H_

#include "kd/common/kdcpudata/kdcpudata.h"
#include "kd/kdf11_a/cpudata/kdf11_aregisters/kdf11_aregisters.h"
#include "qbus/qbus.h"
#include "types.h"
#include "trace/trace.h"

//
// The class KDF11_ACpuData implements the CpuData interface for the KDF11-A.
//
class KDF11_ACpuData : public KDCpuData<KDF11_ARegisters>
{
public:
	// Functions required by the CpuData interface and not implemented by
	// KDCpuData
	bool stackOverflow () override;

private:
	enum {stackLimit = 0400};

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
	 return (psw_ & PSW_MEM_MGMT_MODE) == KERNEL_MODE;
 }

#endif // _KDF11ACPUDATA_H_