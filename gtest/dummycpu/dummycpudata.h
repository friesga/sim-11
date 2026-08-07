#ifndef _DUMMYCPUDATA_H_
#define _DUMMYCPUDATA_H_

#include "proc/common/cpudata/basecpudata/basecpudata.h"
#include "proc/common/cpudata/singularregisterset/singularregisterset.h"
#include "proc/kd/kd11_na/cpudata/kd11_na_psw.h"

class DummyCpuData : public BaseCpuData<SingularRegisterSet, KD11_NA_PSW>
{
public:
    bool const stackOverflow () { return false; };
};


#endif // _DUMMYCPUDATA_H_