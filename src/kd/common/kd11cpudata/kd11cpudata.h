#ifndef _KD11CPUDATA_H_
#define _KD11CPUDATA_H_

#include "kd/include/cpudata.h"
#include "qbus/qbus.h"
#include "types.h"

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
	void setTrap (InterruptRequest const *ir) override;
	void setCC (ConditionCodes conditionCodes) override;

protected:
	Qbus *bus_;
	u16	psw_;

	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time.
	InterruptRequest const *trap_;

	InterruptRequest const busError 
		{RequestType::Trap, TrapPriority::BusError, 0, 004};
	InterruptRequest const illegalInstructionTrap
		{RequestType::Trap, TrapPriority::InstructionTrap, 0, 010};
	InterruptRequest const traceTrap 
		{RequestType::Trap, TrapPriority::TraceTrap, 0, 014};
	InterruptRequest const BreakpointTrap
		{RequestType::Trap, TrapPriority::InstructionTrap, 0, 014};
	InterruptRequest const InputOutputTrap 
		{RequestType::Trap, TrapPriority::InstructionTrap, 0, 020};
	InterruptRequest const EmulatorTrap
		{RequestType::Trap, TrapPriority::InstructionTrap, 0, 030};
	InterruptRequest const TrapInstruction
		{RequestType::Trap, TrapPriority::InstructionTrap, 0, 034};
	InterruptRequest const FIS
		{RequestType::Trap, TrapPriority::InstructionTrap, 0, 0244};

	/*
	// This array will contain pointers to the InterruptRequest's defined
	// above in the order defined in CpuData::Trap enum.
	InterruptRequest const* vectorTable[8]
	{
		nullptr,
		&busError,
		&illegalInstructionTrap,
		&BreakpointTrap,
		&InputOutputTrap,
		&EmulatorTrap,
		&TrapInstruction,
		&FIS
	};
	*/
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