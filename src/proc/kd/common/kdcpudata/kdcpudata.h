#ifndef _KDCPUDATA_H_
#define _KDCPUDATA_H_

#include "proc/kd/include/cpudata.h"
#include "types.h"
#include "trace/trace.h"

#include <functional>
#include <map>
#include <optional>

using std::map;
using std::make_pair;
using std::optional;

//
// The class BaseCpuData is a base implementation for the processor-specific
// CpuData classes. This class is defined to prevent code duplication.
//
template <typename REGISTERTYPE, typename PSWTYPE>
class BaseCpuData : public CpuData
{
public:
	BaseCpuData ();

	// Functions required by the CpuData interface
	constexpr GeneralRegisters& registers () override;
	constexpr PSW& psw () override;
	void setCC (ConditionCodes conditionCodes) override;

	bool trapPending () const override;
	bool trapPending (TrapType trap) const override;
	void setTrap (CpuData::TrapType trap, TrapRecordType cause = TrapRecordType::TRAP) override;
	constexpr void clearTrap () override;
	u16 trapVector () override;
	u16 trapVector (TrapType trap) override;

protected:
	PSWTYPE psw_ {0};
	REGISTERTYPE registers_ {psw_};

private:
	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time. Trap piorities are based
	// on the LSI-11 PDP-11/03 processor handbook p. 4-70:
	// - Bus Error Trap
	// - Memory Refresh
	// - Instruction Traps
	// - Trace Trap
	// - Halt Line
	// - Power Fail Trap
	// - Event Line Interrupt
	// - Device (Bus) Interrupt Request
	//
	// Device (Bus) interrupt requests always have a lower priority than any
	// trap and are modelled in a separate InterruptRequest class.
	// 
	// Note the difference between reserved and illegal instructions. Reserved
	// instructions always trap to vector address 010 (LSI-11/PDP-11/03
	// Processor Handbook page 4-69). Illegal instructions trap to either
	// vector address 004 or 010. On the KD11-NA and KDF11-A processors
	// illegal instructions trap to 004 but on some other processors these
	// instructions trap to vector address 010. (See PDP-11 Architecture
	// Handbook, appendix B, item 5).
	//
	// Definition of the trap vectors and priorities belonging for the trap
	// types.
	//
	struct TrapData
	{
		u16 vectorAddress;
		u16 priority;
	};

	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time.
	optional<CpuData::TrapType> pendingTrap_;

	static map<CpuData::TrapType, TrapData> trapData_;
};

// Constructor
template <typename REGISTERTYPE, typename PSWTYPE>
BaseCpuData<REGISTERTYPE, PSWTYPE>::BaseCpuData ()
    :
    psw_ {0},
    pendingTrap_ {}
{}

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The function psw() is required by the CpuData interface.
//
template <typename REGISTERTYPE, typename PSWTYPE>
constexpr PSW& BaseCpuData<REGISTERTYPE, PSWTYPE>::psw ()
{
	return psw_;
}

// Calculate the new condition codes from the current condition codes and
// the condition codes resulting from execution of an instruction and
// assign these to the condition codes in the PSW.
template <typename REGISTERTYPE, typename PSWTYPE>
void BaseCpuData<REGISTERTYPE, PSWTYPE>::setCC (ConditionCodes conditionCodes)
{
    psw_ = (psw_ & ~(PSW_C | PSW_V | PSW_Z | PSW_N)) | 
        conditionCodes.updatedConditionCodes (psw_);
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr GeneralRegisters& BaseCpuData<REGISTERTYPE, PSWTYPE>::registers ()
{
	return registers_;
}

// Traps are prioritized and the given trap can only be set when it has
// a higher priority than the pending trap.
template <typename REGISTERTYPE, typename PSWTYPE>
void BaseCpuData<REGISTERTYPE, PSWTYPE>::setTrap (CpuData::TrapType trap, TrapRecordType cause)
{
	if (!pendingTrap_.has_value () ||
		trapData_[trap].priority > trapData_[pendingTrap_.value ()].priority)
	{
		trace.trap (cause, trapVector (trap));
		pendingTrap_ = trap;
	}
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr void BaseCpuData<REGISTERTYPE, PSWTYPE>::clearTrap ()
{
	pendingTrap_ = {};
}

template <typename REGISTERTYPE, typename PSWTYPE>
bool BaseCpuData<REGISTERTYPE, PSWTYPE>::trapPending () const
{
	return pendingTrap_.has_value ();
}

// This function throws a bad_optional_access exception when no trap is
// pending. The caller must ensure that a trap is pending before calling
// this function.
template <typename REGISTERTYPE, typename PSWTYPE>
bool BaseCpuData<REGISTERTYPE, PSWTYPE>::trapPending (TrapType trap) const
{
	return pendingTrap_.has_value () && pendingTrap_.value () == trap;
}

template <typename REGISTERTYPE, typename PSWTYPE>
u16 BaseCpuData<REGISTERTYPE, PSWTYPE>::trapVector ()
{
	return trapData_[pendingTrap_.value ()].vectorAddress;
}

template <typename REGISTERTYPE, typename PSWTYPE>
u16 BaseCpuData<REGISTERTYPE, PSWTYPE>::trapVector (TrapType trap)
{
	return trapData_[trap].vectorAddress;
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
map<CpuData::TrapType, 
	typename BaseCpuData<REGISTERTYPE, PSWTYPE>::TrapData>
BaseCpuData<REGISTERTYPE, PSWTYPE>::trapData_
{
	make_pair (CpuData::TrapType::StackOverflow, TrapData {004, 7}),			// Stack overflow
	make_pair (CpuData::TrapType::MemoryManagementTrap, TrapData {0250, 7}),	// Memory Management abort
	make_pair (CpuData::TrapType::BusError, TrapData {004, 6}),					// Time out and other errors
	make_pair (CpuData::TrapType::IllegalInstructionTrap, TrapData {004, 5}),	// Illegal instructions
	make_pair (CpuData::TrapType::ReservedInstructionTrap, TrapData {010, 5}),	// Reserved instructions
	make_pair (CpuData::TrapType::ParityError, TrapData {0114, 5}),				// Memory parity error
	make_pair (CpuData::TrapType::InputOutputTrap, TrapData {020, 4}),			// IOT instruction
	make_pair (CpuData::TrapType::EmulatorTrap, TrapData {030, 4}),				// EMT instruction
	make_pair (CpuData::TrapType::TrapInstruction, TrapData {034, 4}),			// TRAP instruction
	make_pair (CpuData::TrapType::BreakpointTrap, TrapData {014, 3}),			// BPT instruction
	make_pair (CpuData::TrapType::FIS, TrapData {0244, 3}),						// Floating point
	make_pair (CpuData::TrapType::PowerFail, TrapData {024, 1}),				// Power fail
};

#endif // _KDCPUDATA_H_