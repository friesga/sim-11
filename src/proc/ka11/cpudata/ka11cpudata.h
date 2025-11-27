#ifndef _KD11NACPUDATA_H_
#define _KD11NACPUDATA_H_

#include "proc/common/cpudata/basecpudata/basecpudata.h"
#include "proc/common/cpudata/singularregisterset/singularregisterset.h"
#include "ka11_psw.h"
#include "types.h"
#include "trace/trace.h"

//
// The class KA11CpuData implements the CpuData interface for the KA11
//
class KA11CpuData : public BaseCpuData<SingularRegisterSet, KA11_PSW>
{
public:
	// Functions required by the CpuData interface and not implemented by
	// BaseCpuData.
	constexpr bool stackOverflow () override;
};

// The KD11-NA does not support a stack limit so stack overflow cannot occur.
constexpr bool KA11CpuData::stackOverflow ()
{
    return false;
}

#endif // _KD11NACPUDATA_H_