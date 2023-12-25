#ifndef _KD11NACPUDATA_H_
#define _KD11NACPUDATA_H_

#include "kd/common/kdcpudata/kdcpudata.h"
#include "kd11_naregisters/kd11_naregisters.h"
#include "kd11_na_psw.h"
#include "types.h"
#include "trace/trace.h"

//
// The class KD11_NACpuData implements the CpuData interface for the KD11-NA.
//
class KD11_NACpuData : public KDCpuData<KD11_NARegisters, KD11_NA_PSW>
{
public:
	// Functions required by the CpuData interface and not implemented by
	// KDCpuData.
	constexpr bool stackOverflow () override;
};

// The KD11-NA does not support a stack limit so stack overflow cannot occur.
constexpr bool KD11_NACpuData::stackOverflow ()
{
    return false;
}

#endif // _KD11NACPUDATA_H_