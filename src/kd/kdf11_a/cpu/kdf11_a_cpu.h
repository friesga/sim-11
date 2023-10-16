#ifndef _KDF11_A_CPU_H_
#define _KDF11_A_CPU_H_

#include "kd/common/kd11cpudata/kd11cpudata.h"
#include "float/float.h"
#include "types.h"
#include "kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.h"
#include "kdf11_aregisters/kdf11_aregisters.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
class KD11_NA_ODT;
class KD11_NA;

#define USE_FLOAT

// 
// This class simulates a KDF11-A.
//
// The class CpuData is derived from CpuControl and CpuExecution, so the
// KDF11_A_Cpu has to implement all three interfaces.
//
class KDF11_A_Cpu : public KD11CpuData
{
public:
	// The KDF11_A needs access to the PSW. The ControlLogic and LSI11 classes
	// need access to the CpuControl functions.
	friend class KDF11_A;
	friend class ControlLogic;
	friend class KD11_NA_ODT;
	friend class LSI11;
	
	KDF11_A_Cpu (Qbus *bus);

	// This function is required by the CpuExecution interface and executes
	// the next instruction on the cpu.
	bool step () override;

	// Definition of the functions required by the CpuData interface
	constexpr GeneralRegisters& registers () override;

private:
	// Definition of CPU run states
	enum class CpuRunState
	{
		HALT,
		RUN,
		WAIT,
		INHIBIT_TRACE
	};

	CpuRunState runState;
	KDF11_AInstruction kdf11_aInstruction;
	KDF11_ARegisters registers_ {psw_};

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

	HaltReason haltReason_;

	// Definition of CpuControl functions. These functions are
	// used by K11ODT and the Operate Group instructions.
	void loadTrapVector (InterruptRequest const* trap) override;
	void cpuReset () override;
	void busReset () override;
	void halt () override;
	void wait () override;
    void start (u16 address) override;
	void proceed () override;
	void inhibitTraceTrap () override;
    
    constexpr u16 pswValue ();
	constexpr void setPSW (u16 value);
	constexpr HaltReason haltReason ();

	void execInstr ();
	void handleTraps();
	u8 cpuPriority ();
};


constexpr GeneralRegisters& KDF11_A_Cpu::registers ()
{
	return registers_;
}

// The functions setPSW() and pswValue() are used by ODT.
// 
// Set the Processor Status Word to the given value. The T-bit cannot be set
// via this function.
// 
 constexpr void KDF11_A_Cpu::setPSW (u16 value)
 {
     psw_ = (psw_ & PSW_T) | (value & ~PSW_T);
 }

 constexpr u16 KDF11_A_Cpu::pswValue ()
 {
     return psw_;
 }

 constexpr CpuControl::HaltReason KDF11_A_Cpu::haltReason ()
 {
	 return haltReason_;
 }
#endif // _KDF11_A_CPU_H_