#ifndef _KDF11_A_CPU_H_
#define _KDF11_A_CPU_H_

#include "kd/common/kd11cpudata/kd11cpudata.h"
#include "float/float.h"
#include "types.h"
#include "kd/kd11_na/cpu/kd11_nainstruction/kd11_nainstruction.h"

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
	// The ControlLogic and LSI11 classes need access to the CpuControl functions.
	friend class ControlLogic;
	friend class KD11_NA_ODT;
	friend class LSI11;
	
	KDF11_A_Cpu (Qbus *bus);

	// This function is required by the CpuExecution interface and executes
	// the next instruction on the cpu.
	bool step () override;

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
	KD11_NAInstruction kd11_naInstruction;

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
    
	constexpr u16 registerValue (u8 registerNr);
    constexpr void setRegister (u8 registerNr, u16 value);
    constexpr u16 pswValue ();
	constexpr void setPSW (u16 value);
	constexpr HaltReason haltReason ();

	void execInstr ();
	void handleTraps();
	u8 cpuPriority ();
};


// The functions registerValue(), setRegister(), setPSW() and pswValue()
// are used by ODT.
// 
// Return the value of a register. Access to the registers and PSW has to be
// provided via special functions as the KD11 has no registers to access them.
//
constexpr u16 KDF11_A_Cpu::registerValue (u8 registerNr)
{
    return register_[registerNr];
}

// Set the given register to the given value
constexpr void KDF11_A_Cpu::setRegister (u8 registerNr, u16 value)
{
    register_[registerNr] = value;
}

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