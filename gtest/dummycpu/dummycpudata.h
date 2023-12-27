#ifndef _DUMMYCPUDATA_H_
#define _DUMMYCPUDATA_H_

#include "kd/common/kdcpudata/kdcpudata.h"
#include "kd/kd11_na/cpudata/kd11_naregisters/kd11_naregisters.h"
#include "kd/kd11_na/cpudata/kd11_na_psw.h"

class DummyCpuData : public KDCpuData<KD11_NARegisters, KD11_NA_PSW>
{
public:
    bool stackOverflow () { return false; };
};


#endif // _DUMMYCPUDATA_H_