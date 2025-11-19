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

	constexpr TrapType trap () override;
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
		TrapType type;        // which trap occurred
		u16 vector;           // trap vector
		u8 priority;          // usually fixed per trap
	};

	optional<TrapType> pendingTrap_ {};

	static inline constexpr TrapData cpuTrapTable_[]
	{
		TrapData {TrapType::StackOverflow, 004, 7},
		TrapData {TrapType::MemoryManagement, 004, 7},
		TrapData {TrapType::BusError, 004, 6},
		TrapData {TrapType::IllegalInstruction, 004, 5},
		TrapData {TrapType::ReservedInstruction, 010, 5},
		TrapData {TrapType::ParityError, 004, 5},
		TrapData {TrapType::Breakpoint, 014, 4},
		TrapData {TrapType::InputOutput, 020, 3},
		TrapData {TrapType::Emulator, 030, 3},
		TrapData {TrapType::TrapInstruction, 034, 3},
		TrapData {TrapType::PowerFail, 024, 1},
	};
};

// Constructor
template <typename REGISTERTYPE, typename PSWTYPE>
KDCpuData<REGISTERTYPE, PSWTYPE>::KDCpuData ()
    :
    psw_ {0}
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
void KDCpuData<REGISTERTYPE, PSWTYPE>::setTrap (CpuData::TrapType trap, TrapRecordType cause)
{
    trace.trap (cause, trapVector (trap));
    pendingTrap_ = trap;
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr void KDCpuData<REGISTERTYPE, PSWTYPE>::clearTrap ()
{
	pendingTrap_ = {};
}

template <typename REGISTERTYPE, typename PSWTYPE>
constexpr CpuData::TrapType KDCpuData<REGISTERTYPE, PSWTYPE>::trap ()
{
	return cpuTrapTable_[static_cast<size_t> (pendingTrap_.value ())].type;
}

template <typename REGISTERTYPE, typename PSWTYPE>
u16 KDCpuData<REGISTERTYPE, PSWTYPE>::trapVector ()
{
	return cpuTrapTable_[static_cast<size_t> (pendingTrap_.value ())].vector;
}

template <typename REGISTERTYPE, typename PSWTYPE>
u16 KDCpuData<REGISTERTYPE, PSWTYPE>::trapVector (TrapType trapType)
{
	return cpuTrapTable_[static_cast<size_t> (trapType)].vector;
}

#endif // _KDCPUDATA_H_