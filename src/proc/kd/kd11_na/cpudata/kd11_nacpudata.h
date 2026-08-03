#ifndef _KD11NACPUDATA_H_
#define _KD11NACPUDATA_H_

#include "proc/common/cpudata/basecpudata/basecpudata.h"
#include "proc/common/cpudata/singularregisterset/singularregisterset.h"
#include "kd11_na_psw.h"
#include "types.h"
#include "trace/trace.h"

//
// The class KD11_NACpuData implements the CpuData interface for the KD11-NA.
//
class KD11_NACpuData : public BaseCpuData<SingularRegisterSet, KD11_NA_PSW>
{
public:
	// Functions required by the CpuData interface and not implemented by
	// BaseCpuData.
	const bool stackOverflow () override;
};

// The KD11-NA does not support a stack limit so stack overflow cannot occur.
inline const bool KD11_NACpuData::stackOverflow ()
{
    return false;
}

#endif // _KD11NACPUDATA_H_