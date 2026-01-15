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
    constexpr bool stackOverflow () override;
};

// The KA11 does not support a stack limit so stack overflow cannot occur.
constexpr bool KA11CpuData::stackOverflow ()
{
    return false;
}

#endif // _KA11CPUDATA_H_