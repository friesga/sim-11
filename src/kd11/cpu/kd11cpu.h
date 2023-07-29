#ifndef _KD11CPU_H_
#define _KD11CPU_H_

#include "cpucontrol.h"
#include "qbus/qbus.h"
#include "float/float.h"
#include "types.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
namespace KD11_ODT { class KD11ODT; }
namespace kd11_na { class KD11; }

// Definition of CPU states
enum class CpuState
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

#define	PSW_GET(x)		(((psw) & (x)) ? 1 : 0)
#define	PSW_SET(x)		((psw) |= (x))
#define	PSW_CLR(x)		((psw) &= ~(x))
#define	PSW_EQ(x, v) { \
	if(v) { \
		PSW_SET(x); \
	} else { \
		PSW_CLR(x); \
	} \
}

#define USE_FLOAT

// Two different LSI-models exist, the LSI-11 and the LSI-11/2. The LSI-11
// comprises the M7264 module in one of its variations. The LSI-11/2
// consists of a M7270 module with a KD11-HA or KD11-NA processor. These
// processors differ in the availability of the EIS and FIS options.
// See http://web.frainresearch.org:8080/projects/pdp-11/lsi-11.php for
// an overview of the different variations. 
// This class simulates a KD11-NA, i.e. a KD11-H base version including EIS
// and FIS support.
class KD11CPU : public CpuControl
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
	CpuState currentCpuState ();

private:
	Qbus *bus_;
	u16	register_[8];
	u16	psw;
	CpuState runState;

	// A trap is a special kind of interrupt, internal to the CPU. There
	// can be only one trap serviced at the time.
	InterruptRequest const *trap_;

	InterruptRequest const busError{RequestType::Trap, TrapPriority::BusError, 0, 004};
	InterruptRequest const traceTrap{RequestType::Trap, TrapPriority::TraceTrap, 0, 014};
	InterruptRequest const BPT{RequestType::Trap, TrapPriority::InstructionTrap, 0, 014};
	InterruptRequest const IOT{RequestType::Trap, TrapPriority::InstructionTrap, 0, 020};
	InterruptRequest const EmulatorTrap {RequestType::Trap, TrapPriority::InstructionTrap, 0, 030};
	InterruptRequest const TRP{RequestType::Trap, TrapPriority::InstructionTrap, 0, 034};
	InterruptRequest const FIS{RequestType::Trap, TrapPriority::InstructionTrap, 0, 0244};
	InterruptRequest const illegalInstructionTrap{RequestType::Trap, TrapPriority::InstructionTrap, 0, 010};

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
    u16 registerValue (u8 registerNr);
    void setRegister (u8 registerNr, u16 value);
    u16 pswValue ();
    void setPSW (u16 value);

	// These functions are used by the Instruction classes.
	CondData<u16> fetchWord (u16 address);
	CondData<u8> fetchByte (u16 address);
	bool putWord (u16 address, u16 value);
	bool putByte (u16 address, u8 value);

	void execInstr ();
	void handleTraps();
	u8 cpuPriority ();
	void returnFISresult (Float result, u16 registerNumber);
	void executeFISinstruction (u16 stackPointer, 
		std::function<bool(Float, Float)> argumentsValid,
		std::function<Float(Float, Float)>);

	bool popWord (u16 *destination);

	// Instruction in the Program Control Group Traps and Operate Group.
	// These instruction have no operands.
	void HALT ();
	void EMT ();
	void TRAP ();
	void unused ();

	// Single operand instructions
	void JMP (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void SWAB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void CLR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void CLRB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void COM (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void COMB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void INC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void INCB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void DEC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void DECB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void NEG (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void NEGB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ADC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ADCB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void SBC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void SBCB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void TST (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void TSTB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ROR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void RORB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ROL (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ROLB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ASR (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ASRB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ASL (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ASLB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void SXT (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);

	// Double operand instructions
	void MOV (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void MOVB (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void CMP (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void BIT (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void BIC (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void BIS (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void ADD (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);

	// EIS, JSR and XOR instructions. These instructions share the same
	// instruction format
	void JSR (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void MUL (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void DIV (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void ASH (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void ASHC (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void XOR (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);

	// FIS and RTS instructions. These instructions share the FIS instruction
	// format.
	void RTS (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void FADD (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void FSUB (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void FMUL (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);
	void FDIV (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);

	// SOB instruction format
	void SOB (KD11CPU *cpu, u16 (&reg)[8], u16 instruction);

	void MARK (KD11CPU* cpu, u16 (&reg)[8], u16 instruction);
	void CCC (u16 &psw, u16 instruction);
	void SCC (u16 &psw, u16 instruction);

	// Branch instructions
	void BR (u16 instruction);
	void BNE (u16 instruction);
	void BEQ (u16 instruction);
	void BGE (u16 instruction);
	void BLT (u16 instruction);
	void BGT (u16 instruction);
	void BLE (u16 instruction);
	void BPL (u16 instruction);
	void BMI (u16 instruction);
	void BHI (u16 instruction);
	void BLOS (u16 instruction);
	void BVC (u16 instruction);
	void BVS (u16 instruction);
	void BCC (u16 instruction);
	void BCS (u16 instruction);
};


#endif // _KD11CPU_H_