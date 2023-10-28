#ifndef _KD11CPUDATA_H_
#define _KD11CPUDATA_H_

#include "kd/include/cpudata.h"
#include "qbus/qbus.h"
#include "types.h"
#include "trace/trace.h"

#include <functional>

//
// The class CpuData implements the CpuData interface and can act as a base
// class for the KD11_NA and KDF11_A classes, thus preventing code duplication.
//
class KD11CpuData : public CpuData
{
public:
	KD11CpuData (Qbus *bus);

	// These functions have to be provided for the CpuData interfaces and are
	// used by the instruction classes.
    constexpr u16& psw () override;
	CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;
	bool pushWord (u16 value) override;
	bool popWord (u16 *destination) override;
	void setTrap (CpuData::Trap trap, TrapRecordType cause = TrapRecordType::TRAP) override;
	void setCC (ConditionCodes conditionCodes) override;

protected:
	Qbus *bus_;
	u16	psw_;

	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time.
	CpuData::Trap trap_;
};

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The function psw() is required by the CpuData interface.
//
constexpr u16& KD11CpuData::psw ()
{
	return psw_;
}

#endif // _KD11CPUDATA_H_