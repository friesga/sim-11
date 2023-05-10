#ifndef _KD11CPU_H_
#define _KD11CPU_H_

#include "cpucontrol.h"
#include "qbus/qbus.h"
#include "float/float.h"
#include "types.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
namespace KD11_ODT { class KD11ODT; }
namespace kd11_f { class KD11; }

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

class KD11CPU : public CpuControl
{
public:
	// KD11ODT and LSI11 need access to the CpuControl functions. The class
	// Instruction needs access to fetchWord(), putWord() and putByte().
	// I prefer to declare these classes friends to making these functions
	// public.
	friend class kd11_f::KD11;
	friend class KD11_ODT::KD11ODT;
	friend class LSI11;
	friend class Instruction;

	KD11CPU (Qbus *bus);
	bool step ();
	CpuState currentCpuState ();

private:
	enum {bootAddress = 0173000};

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
	InterruptRequest const EMT{RequestType::Trap, TrapPriority::InstructionTrap, 0, 030};
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
	void setTrap (InterruptRequest const *ir);
	void reset ();
	void halt ();
    void start (u16 address);
	void proceed ();
    u16 registerValue (u8 registerNr);
    void setRegister (u8 registerNr, u16 value);
    u16 pswValue ();
    void setPSW (u16 value);

	// These functions are used by the
	// Instruction classes.
	CondData<u16> fetchWord (u16 address);
	bool putWord (u16 address, u16 value);
	bool putByte (u16 address, u8 value);

	void execInstr ();
	void handleTraps();
	u8 cpuPriority ();
	void returnFISresult (Float result, u16 registerNumber);
	void executeFISinstruction (u16 stackPointer, 
		std::function<bool(Float, Float)> argumentsValid,
		std::function<Float(Float, Float)>);
};


#endif // _KD11CPU_H_