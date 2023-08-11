#ifndef _KD11CPU_H_
#define _KD11CPU_H_

#include "cpucontrol.h"
#include "cpudata.h"
#include "qbus/qbus.h"
#include "float/float.h"
#include "types.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
namespace KD11_ODT { class KD11ODT; }
namespace kd11_na { class KD11; }

// Definition of CPU run states
enum class CpuRunState
{
	HALT,
	RUN,
	WAIT,
	INHIBIT_TRACE
};

// The processor status word in the LSI-11 is a composite of (1) the 4 PDP-11
// condition codes (N,Z,V,C) [bits 0:3] and (2) the Trace Bit [bit 4] and (3)
// the Interrupt Enable bit [bit 7]. (EK-KUV11-TM-001 pg. 1-9). 
// Processor Status Word bits 5 and 6 are reserved (EK-KUV11-TM-001
// Figure 2-5). These bits can be set and cleared by the MTPS instruction
// however. At least every FIS instruction resets bits 5 and 6. This
// behaviour is confirmed by the VKAAC0 (Basic Intruction Tests) and VKACC0
// diagnostics.
#define	PSW_C			_BV(0)
#define	PSW_V			_BV(1)
#define	PSW_Z			_BV(2)
#define	PSW_N			_BV(3)
#define	PSW_T			_BV(4)
#define	PSW_PRIO		_BV(7)
#define	PSW_PRIORITY	(_BV(7) | _BV(6) | _BV(5))

#define USE_FLOAT

// Two different LSI-models exist, the LSI-11 and the LSI-11/2. The LSI-11
// comprises the M7264 module in one of its variations. The LSI-11/2
// consists of a M7270 module with a KD11-HA or KD11-NA processor. These
// processors differ in the availability of the EIS and FIS options.
// See http://web.frainresearch.org:8080/projects/pdp-11/lsi-11.php for
// an overview of the different variations. 
// This class simulates a KD11-NA, i.e. a KD11-H base version including EIS
// and FIS support.
//
class KD11CPU : public CpuControl, public CpuData
{
public:
	// KD11ODT and LSI11 need access to the CpuControl functions. The class
	// Instruction needs access to fetchWord(), putWord() and putByte().
	// I prefer to declare these classes friends to making these functions
	// public.
	friend class kd11_na::KD11;
	friend class KD11_ODT::KD11ODT;
	friend class LSI11;
	friend class Instruction;
	friend class LSI11Instruction;
	friend class EisInstruction;
	friend class OperandLocation;

	KD11CPU (Qbus *bus);
	bool step ();
	CpuRunState currentCpuState ();

	// These functions have to be provided for the CpuData interfaces and are
	// used by the instruction classes.
	constexpr GeneralRegisters& registers () override;
    constexpr u16& psw () override;
	CondData<u16> fetchWord (u16 address) override;
	CondData<u8> fetchByte (u16 address) override;
	bool putWord (u16 address, u16 value) override;
	bool putByte (u16 address, u8 value) override;

private:
	Qbus *bus_;
	u16	register_[8];
	u16	psw_;
	CpuRunState runState;

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
	InterruptRequest const TRP
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
		&TRP,
		&FIS
	};

	// This enum holds the reason for halting the system. This data is printed
	// by the ODT Maintenance command.
	// (LSI11 PDP11/03 Processor Handbook)
	enum class HaltReason
	{
		HaltInstruction			= 0,	// Halt instruction or B Halt line
		BusErrorOnIntrptVector  = 1,	// Bus Error occurred while getting device interrupt vector
		BusErrorOnMemoryRefresh = 2,	// Not used
		DoubleBusError			= 3,	// Double Bus Error occurred (stack was non-existent value)
		NonExistentMicroAddress = 4		// Not used
	};

	HaltReason haltReason_;

	// Definition of CpuControl functions. The intention is that these
	// functions are to be used by K11ODT and the reset function.
	void loadTrapVector (InterruptRequest const* trap);
	void setTrap (InterruptRequest const *ir);
	void reset ();
	void halt ();
    void start (u16 address);
	void proceed ();
    
	constexpr u16 registerValue (u8 registerNr);
    constexpr void setRegister (u8 registerNr, u16 value);
    constexpr u16 pswValue ();
	constexpr void setPSW (u16 value);
	constexpr bool isSet (u16 x);
	constexpr void setConditionCode (u16 x);
	constexpr void clearConditionCode (u16 x);
	constexpr void setConditionCodeIf_ClearElse (u16 x, bool v);

	void execInstr ();
	void handleTraps();
	u8 cpuPriority ();
	void returnFISresult (Float result, u16 registerNumber);
	void executeFISinstruction (u16 stackPointer, 
		std::function<bool(Float, Float)> argumentsValid,
		std::function<Float(Float, Float)>);
	constexpr void executeBranchIf (bool condition, CpuData*  cpu, u16 instruction);

	bool popWord (u16 *destination);

	// Instruction in the Program Control Group Traps and Operate Group.
	// These instruction have no operands.
	void HALT (CpuData* cpu, u16 instruction);
	void WAIT (CpuData* cpu, u16 instruction);
	void RTI (CpuData* cpu, u16 instruction);
	void BPT (CpuData* cpu, u16 instruction);
	void IOT (CpuData* cpu, u16 instruction);
	void RESET (CpuData* cpu, u16 instruction);
	void RTT (CpuData* cpu, u16 instruction);
	void EMT (CpuData* cpu, u16 instruction);
	void TRAP (CpuData* cpu, u16 instruction);
	void unused (CpuData* cpu, u16 instruction);

	// Single operand instructions
	void JMP (CpuData *cpu, u16 instruction);
	void SWAB (CpuData* cpu, u16 instruction);
	void CLR (CpuData* cpu, u16 instruction);
	void CLRB (CpuData* cpu, u16 instruction);
	void COM (CpuData* cpu, u16 instruction);
	void COMB (CpuData* cpu, u16 instruction);
	void INC (CpuData* cpu, u16 instruction);
	void INCB (CpuData* cpu, u16 instruction);
	void DEC (CpuData* cpu, u16 instruction);
	void DECB (CpuData* cpu, u16 instruction);
	void NEG (CpuData* cpu, u16 instruction);
	void NEGB (CpuData* cpu, u16 instruction);
	void ADC (CpuData* cpu, u16 instruction);
	void ADCB (CpuData* cpu, u16 instruction);
	void SBC (CpuData* cpu, u16 instruction);
	void SBCB (CpuData* cpu, u16 instruction);
	void TST (CpuData* cpu, u16 instruction);
	void TSTB (CpuData* cpu, u16 instruction);
	void ROR (CpuData* cpu, u16 instruction);
	void RORB (CpuData* cpu, u16 instruction);
	void ROL (CpuData* cpu, u16 instruction);
	void ROLB (CpuData* cpu, u16 instruction);
	void ASR (CpuData* cpu, u16 instruction);
	void ASRB (CpuData* cpu, u16 instruction);
	void ASL (CpuData* cpu, u16 instruction);
	void ASLB (CpuData* cpu, u16 instruction);
	void MTPS (CpuData* cpu, u16 instruction);
	void MFPS (CpuData* cpu, u16 instruction);
	void SXT (CpuData* cpu, u16 instruction);

	// Double operand instructions
	void MOV (CpuData* cpu, u16 instruction);
	void MOVB (CpuData* cpu, u16 instruction);
	void CMP (CpuData* cpu, u16 instruction);
	void CMPB (CpuData* cpu, u16 instruction);
	void BIT (CpuData* cpu, u16 instruction);
	void BITB (CpuData* cpu, u16 instruction);
	void BIC (CpuData* cpu, u16 instruction);
	void BICB (CpuData* cpu, u16 instruction);
	void BIS (CpuData* cpu, u16 instruction);
	void BISB (CpuData* cpu, u16 instruction);
	void ADD (CpuData* cpu, u16 instruction);
	void SUB (CpuData* cpu, u16 instruction);

	// EIS, JSR and XOR instructions. These instructions share the same
	// instruction format
	void JSR (CpuData* cpu, u16 instruction);
	void MUL (CpuData* cpu, u16 instruction);
	void DIV (CpuData* cpu, u16 instruction);
	void ASH (CpuData* cpu, u16 instruction);
	void ASHC (CpuData* cpu, u16 instruction);
	void XOR (CpuData* cpu, u16 instruction);

	// FIS and RTS instructions. These instructions share the FIS instruction
	// format.
	void RTS (CpuData* cpu, u16 instruction);
	void FADD (CpuData* cpu, u16 instruction);
	void FSUB (CpuData* cpu, u16 instruction);
	void FMUL (CpuData* cpu, u16 instruction);
	void FDIV (CpuData* cpu, u16 instruction);

	// SOB instruction format
	void SOB (CpuData* cpu, u16 instruction);

	void MARK (CpuData*  cpu, u16 instruction);
	void CCC (CpuData* cpu, u16 instruction);
	void SCC (CpuData* cpu, u16 instruction);

	// Branch instructions
	void BR (CpuData*  cpu, u16 instruction);
	void BNE (CpuData*  cpu, u16 instruction);
	void BEQ (CpuData*  cpu, u16 instruction);
	void BGE (CpuData*  cpu, u16 instruction);
	void BLT (CpuData*  cpu, u16 instruction);
	void BGT (CpuData*  cpu, u16 instruction);
	void BLE (CpuData*  cpu, u16 instruction);
	void BPL (CpuData*  cpu, u16 instruction);
	void BMI (CpuData*  cpu, u16 instruction);
	void BHI (CpuData*  cpu, u16 instruction);
	void BLOS (CpuData*  cpu, u16 instruction);
	void BVC (CpuData*  cpu, u16 instruction);
	void BVS (CpuData*  cpu, u16 instruction);
	void BCC (CpuData*  cpu, u16 instruction);
	void BCS (CpuData*  cpu, u16 instruction);
};

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The functions registers() and psw() are required by the CpuData interface.
//
constexpr CpuData::GeneralRegisters& KD11CPU::registers ()
{
	return register_;
}

constexpr u16& KD11CPU::psw ()
{
	return psw_;
}

// ToDo: Move condition code functions to LSI11Instruction
constexpr bool KD11CPU::isSet (u16 x)
{
	return (psw_ & x) ? true : false;
}

constexpr void KD11CPU::setConditionCode (u16 x)
{
	psw_ |= x;
}

constexpr void KD11CPU::clearConditionCode (u16 x)
{
	psw_ &= ~x;
}

constexpr void KD11CPU::setConditionCodeIf_ClearElse (u16 x, bool condition)
{
	if (condition)
		setConditionCode (x);
	else
		clearConditionCode (x);
}

// The functions registerValue(), setRegister(), setPSW() and pswValue()
// are used by ODT.
// 
// Return the value of a register. Access to the registers and PSW has to be
// provided via special functions as the KD11 has no registers to access them.
//
constexpr u16 KD11CPU::registerValue (u8 registerNr)
{
    return register_[registerNr];
}

// Set the given register to the given value
constexpr void KD11CPU::setRegister (u8 registerNr, u16 value)
{
    register_[registerNr] = value;
}

// Set the Processor Status Word to the given value. The T-bit cannot be set
// via this function.
// 

 constexpr void KD11CPU::setPSW (u16 value)
 {
     psw_ = (psw_ & PSW_T) | (value & ~PSW_T);
 }

 constexpr u16 KD11CPU::pswValue ()
 {
     return psw_;
 }

#endif // _KD11CPU_H_