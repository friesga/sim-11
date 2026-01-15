#ifndef _KA11CPUDATA_H_
#define _KA11CPUDATA_H_

#include "proc/common/cpudata/basecpudata/basecpudata.h"
#include "proc/common/cpudata/singularregisterset/singularregisterset.h"
#include "ka11_psw.h"
#include "types.h"
#include "trace/trace.h"
#include "abstractbusdevice/abstractbusdevice.h"

//
// The class KA11CpuData implements the CpuData interface for the KA11
//
class KA11CpuData : public BaseCpuData<SingularRegisterSet, KA11_PSW>,
    public AbstractBusDevice
{
public:
    // Functions required by the CpuData interface and not implemented by
    // BaseCpuData.
    constexpr bool stackOverflow () override;

    // Functions required by the BusDevice interface and not implemented by
    // AbstractBusDevice.
    CondData<u16> read (BusAddress address) override;
    StatusCode writeWord (BusAddress address, u16 value) override;
    bool responsible (BusAddress address) override;
    void reset () override;

private:
    enum { PSWAddress = 0177776 };
};

// The KA11 does not support a stack limit so stack overflow cannot occur.
constexpr bool KA11CpuData::stackOverflow ()
{
    return false;
}

#endif // _KA11CPUDATA_H_