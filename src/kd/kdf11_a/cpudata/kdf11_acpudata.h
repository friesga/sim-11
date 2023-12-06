#ifndef _KDF11ACPUDATA_H_
#define _KDF11ACPUDATA_H_

#include "kd/include/cpudata.h"
#include "kd/kdf11_a/cpu/kdf11_aregisters/kdf11_aregisters.h"
#include "qbus/qbus.h"
#include "types.h"
#include "trace/trace.h"

#include <functional>
#include <map>

using std::map;

//
// The class KDF11_ACpuData implements the CpuData interface for the KDF11-A.
//
class KDF11_ACpuData : public CpuData
{
public:
	KDF11_NACpuData (Qbus *bus);

	// Functions requited by the CpuData interface
	constexpr GeneralRegisters& registers () override;
	constexpr u16& psw () override;
	void setCC (ConditionCodes conditionCodes) override;
	constexpr bool stackOverflow () override;

	void setTrap (CpuData::TrapCondition trap, TrapRecordType cause = TrapRecordType::TRAP) override;
	u16 trapVector () override;
	u16 trapVector (TrapCondition trap) override;

private:
	Qbus *bus_;
	u16	psw_;
	KD11_NARegisters registers_;

	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time.
	CpuData::TrapCondition trap_;

	static map<CpuData::TrapCondition, u16> trapVector_;
};

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The function psw() is required by the CpuData interface.
//
constexpr u16& KDF11_ACpuData::psw ()
{
	return psw_;
}

constexpr GeneralRegisters& KDF11_ACpuData::registers ()
{
	return registers_;
}

// The KD11-NA does not support a stack limit so stack overflow cannot occur.
constexpr bool KDF11_ACpuData::stackOverflow ()
{
    return false;
}

inline u16 KDF11_ACpuData::trapVector ()
{
	return trapVector_[trap_];
}

inline u16 KDF11_ACpuData::trapVector (TrapCondition trap)
{
	return trapVector_[trap_];
}

#endif // _KDF11ACPUDATA_H_