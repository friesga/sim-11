#ifndef _KD11NACPUDATA_H_
#define _KD11NACPUDATA_H_

#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpudata/kd11_naregisters/kd11_naregisters.h"
#include "qbus/qbus.h"
#include "types.h"
#include "trace/trace.h"

#include <functional>
#include <map>

using std::map;

//
// The class KD11_NACpuData implements the CpuData interface for the KD11-NA.
//
class KD11_NACpuData : public CpuData
{
public:
	KD11_NACpuData ();

	// Functions requited by the CpuData interface
	constexpr GeneralRegisters& registers () override;
	constexpr u16& psw () override;
	constexpr void setPSW (u16 value) override;
	void setCC (ConditionCodes conditionCodes) override;
	constexpr bool stackOverflow () override;

	constexpr TrapCondition trap () override;
	void setTrap (CpuData::TrapCondition trap, TrapRecordType cause = TrapRecordType::TRAP) override;
	constexpr void clearTrap () override;
	u16 trapVector () override;
	u16 trapVector (TrapCondition trap) override;

private:
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
constexpr u16& KD11_NACpuData::psw ()
{
	return psw_;
}

// Set the Processor Status Word to the given value. The T-bit cannot be set
// via this function.
// 
 constexpr void KD11_NACpuData::setPSW (u16 value)
 {
     psw_ = (psw_ & PSW_T) | (value & ~PSW_T);
 }

constexpr GeneralRegisters& KD11_NACpuData::registers ()
{
	return registers_;
}

// The KD11-NA does not support a stack limit so stack overflow cannot occur.
constexpr bool KD11_NACpuData::stackOverflow ()
{
    return false;
}

constexpr void KD11_NACpuData::clearTrap ()
{
	trap_ = TrapCondition::None;
}

constexpr CpuData::TrapCondition KD11_NACpuData::trap ()
{
	return trap_;
}

inline u16 KD11_NACpuData::trapVector ()
{
	return trapVector_[trap_];
}

inline u16 KD11_NACpuData::trapVector (TrapCondition trap)
{
	return trapVector_[trap];
}

#endif // _KD11NACPUDATA_H_