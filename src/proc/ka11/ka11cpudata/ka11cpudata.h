#ifndef _KA11CPUDATA_H_
#define _KA11CPUDATA_H_

#include "proc/common/cpudata/basecpudata/basecpudata.h"
#include "proc/common/cpudata/singularregisterset/singularregisterset.h"
#include "ka11_psw.h"
#include "types.h"
#include "trace/trace.h"
#include "proc/common/cpudata/pswbusdevice/pswbusdevice.h"

//
// The class KA11CpuData provides the KA11-spcific version of the CpuData,
// the BusDevice interface for bus access to the PSW and implements the
// stack overflow functions required by the CpuData interface.
//
class KA11CpuData : public BaseCpuData<SingularRegisterSet, KA11_PSW>,
    public PSWBusDevice<KA11CpuData>
{
public:
    // Functions required by the CpuData interface and not implemented by
    // BaseCpuData.
    const bool stackOverflow () override;

private:
    enum { stackLimit = 0400 };
};

// A Stack Overflow Trap is caused by referencing addresses below 400,
// through the processor stack pointer R6 (SP) in autodecrement or
// autodecrement deferred addressing. The instruction causing the overflow
// is completed before the trap is made.
// Source: PDP-11 Processor Handbook (1969) p. 43.
//
inline const bool KA11CpuData::stackOverflow ()
{
    return registers_[6] < stackLimit;
}

#endif // _KA11CPUDATA_H_