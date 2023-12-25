#ifndef _KDCPUDATA_H_
#define _KDCPUDATA_H_

#include "kd/include/cpudata.h"
#include "types.h"
#include "trace/trace.h"

#include <functional>
#include <map>

using std::map;
using std::make_pair;

//
// The class KDCpuData is a base class for the KD11_NACpuData and
// KDF11_ACpuData classes to prevent code duplication.
//
template <typename REGISTERTYPE, typename PSWTYPE>
class KDCpuData : public CpuData
{
public:
	KDCpuData ();

	// Functions required by the CpuData interface
	constexpr GeneralRegisters& registers () override;
	constexpr PSW& psw () override;
	void setCC (ConditionCodes conditionCodes) override;

	constexpr TrapCondition trap () override;
	void setTrap (CpuData::TrapCondition trap, TrapRecordType cause = TrapRecordType::TRAP) override;
	constexpr void clearTrap () override;
	u16 trapVector () override;
	u16 trapVector (TrapCondition trap) override;

protected:
	PSWTYPE psw_ {0};
	REGISTERTYPE registers_ {psw_};

	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time.
	CpuData::TrapCondition trap_;

	static map<CpuData::TrapCondition, u16> trapVector_;
};

// Constructor
template <typename REGISTERTYPE, typename PSWTYPE>
KDCpuData<REGISTERTYPE, PSWTYPE>::KDCpuData ()
    :
    psw_ {0},
    trap_ {CpuData::TrapCondition::None}
{}

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The function psw() is required by the CpuData interface.
//
template <typename REGISTERTYPE, typename PSWTYPE>
constexpr PSW& KDCpuData<REGISTERTYPE, PSWTYPE>::psw ()
{
	return psw_;
}

// Calculate the new condition codes from the current condition codes and
// the condition codes resulting from execution of an instruction and
// assign these to the condition codes in the PSW.
template <typename REGISTERTYPE, typename PSWTYPE>
void KDCpuData<REGISTERTYPE, PSWTYPE>::setCC (ConditionCodes conditionCodes)
{
    psw_ = (psw_ & ~(PSW_C | PSW_V | PSW_Z | PSW_N)) | 
        conditionCodes.updatedConditionCodes (psw_);
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr GeneralRegisters& KDCpuData<REGISTERTYPE, PSWTYPE>::registers ()
{
	return registers_;
}

// Generate the given trap using the interrupt request mechanism
template <typename REGISTERTYPE, typename PSWTYPE>
void KDCpuData<REGISTERTYPE, PSWTYPE>::setTrap (CpuData::TrapCondition trap, TrapRecordType cause)
{
    trace.trap (cause, trapVector (trap));
    trap_ = trap;
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr void KDCpuData<REGISTERTYPE, PSWTYPE>::clearTrap ()
{
	trap_ = TrapCondition::None;
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr CpuData::TrapCondition KDCpuData<REGISTERTYPE, PSWTYPE>::trap ()
{
	return trap_;
}

template <typename REGISTERTYPE, typename PSWTYPE>
u16 KDCpuData<REGISTERTYPE, PSWTYPE>::trapVector ()
{
	return trapVector_[trap_];
}

template <typename REGISTERTYPE, typename PSWTYPE>
u16 KDCpuData<REGISTERTYPE, PSWTYPE>::trapVector (TrapCondition trap)
{
	return trapVector_[trap];
}

// Definition of the trap vector belonging to the trap condition.
// 
// Note the difference between reserved and illegal instructions. Reserved
// instructions always trap to vector address 010 (LSI-11/PDP-11/03 Processor
// Handbook page 4-69). Illegal instructions trap to either vector address
// 004 or 010. On the KD11-NA and KDF11-A processors illegal instructions trap
// to 004 but on some other processors these instructions trap to vector
// address 010. (See PDP-11 Architecture Handbook, appendix B, item 5).
template <typename REGISTERTYPE, typename PSWTYPE>
map<CpuData::TrapCondition, u16> KDCpuData<REGISTERTYPE, PSWTYPE>::trapVector_ 
{
    make_pair (CpuData::TrapCondition::None, 0),						// Reserved
	make_pair (CpuData::TrapCondition::BusError, 004),					// Time out and other errors
	make_pair (CpuData::TrapCondition::IllegalInstructionTrap, 004),	// Illegal instructions
	make_pair (CpuData::TrapCondition::ReservedInstructionTrap, 010),	// Reserved instructions
	make_pair (CpuData::TrapCondition::BreakpointTrap, 014),			// BPT instruction
	make_pair (CpuData::TrapCondition::InputOutputTrap, 020),			// IOT instruction
	make_pair (CpuData::TrapCondition::PowerFail, 024),					// Power fail
	make_pair (CpuData::TrapCondition::EmulatorTrap, 030),				// EMT instruction
	make_pair (CpuData::TrapCondition::TrapInstruction, 034),			// TRAP instruction
	make_pair (CpuData::TrapCondition::FIS, 0244),						// Floating point
	make_pair (CpuData::TrapCondition::StackOverflow, 004),				// Stack overflow
	make_pair (CpuData::TrapCondition::MemoryManagementTrap, 0250),		// Memory Management abort
};

#endif // _KDCPUDATA_H_