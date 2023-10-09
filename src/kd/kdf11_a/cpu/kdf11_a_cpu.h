#ifndef _KDF11_A_CPU_H_
#define _KDF11_A_CPU_H_

#include "kd/include/cpucontrol.h"
#include "kd/include/cpudata.h"
#include "qbus/qbus.h"
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
class KDF11_A_Cpu : public CpuData
{
public:
	// The ControlLogic and LSI11 classes need access to the CpuControl functions.
	friend class ControlLogic;
	friend class KD11_NA_ODT;
	friend class LSI11;
	
	KDF11_A_Cpu (Qbus *bus);

	// These functions have to be provided for the CpuData interfaces and are
	// used by the instruction classes.
	constexpr GeneralRegisters& registers () override;
    constexpr u16& psw () override;
	CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;
	void pushWord (u16 value) override;
	bool popWord (u16 *destination) override;

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

	Qbus *bus_;
	u16	register_[8];
	u16	psw_;
	CpuRunState runState;
	KD11_NAInstruction kd11_naInstruction;

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
	void setTrap (InterruptRequest const *ir) override;
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

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The functions registers() and psw() are required by the CpuData interface.
//
constexpr CpuData::GeneralRegisters& KDF11_A_Cpu::registers ()
{
	return register_;
}

constexpr u16& KDF11_A_Cpu::psw ()
{
	return psw_;
}


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