#ifndef _KD11_NA_CPU_H_
#define _KD11_NA_CPU_H_

#include "cpucontrol.h"
#include "cpudata.h"
#include "qbus/qbus.h"
#include "float/float.h"
#include "types.h"
#include "kd11_nainstruction/kd11_nainstruction.h"

#include <functional>

// Forward declarations to be able to declare these classes as friends
class KD11_NA_ODT;
namespace kd11_na { class KD11_NA; }

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
// The class CpuData is derived from CpuControl, so the KD11_NA_Cpu has to
// implement both interfaces.
//
class KD11_NA_Cpu : public CpuData
{
public:
	// KD11_NA_ODT and LSI11 need access to the CpuControl functions.
	friend class kd11_na::KD11_NA;
	friend class KD11_NA_ODT;
	friend class LSI11;
	
	KD11_NA_Cpu (Qbus *bus);
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
	void pushWord (u16 value) override;
	bool popWord (u16 *destination) override;

private:
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

	// Definition of CpuControl functions. These functions are
	// used by K11ODT and the Operate Group instructions.
	void setTrap (InterruptRequest const *ir) override;
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

	void loadTrapVector (InterruptRequest const* trap);
	void execInstr ();
	void handleTraps();
	u8 cpuPriority ();
};

// constexpr functions are implicitly inline and therefore need to be defined
// in every translation unit.
//
// The functions registers() and psw() are required by the CpuData interface.
//
constexpr CpuData::GeneralRegisters& KD11_NA_Cpu::registers ()
{
	return register_;
}

constexpr u16& KD11_NA_Cpu::psw ()
{
	return psw_;
}


// The functions registerValue(), setRegister(), setPSW() and pswValue()
// are used by ODT.
// 
// Return the value of a register. Access to the registers and PSW has to be
// provided via special functions as the KD11 has no registers to access them.
//
constexpr u16 KD11_NA_Cpu::registerValue (u8 registerNr)
{
    return register_[registerNr];
}

// Set the given register to the given value
constexpr void KD11_NA_Cpu::setRegister (u8 registerNr, u16 value)
{
    register_[registerNr] = value;
}

// Set the Processor Status Word to the given value. The T-bit cannot be set
// via this function.
// 

 constexpr void KD11_NA_Cpu::setPSW (u16 value)
 {
     psw_ = (psw_ & PSW_T) | (value & ~PSW_T);
 }

 constexpr u16 KD11_NA_Cpu::pswValue ()
 {
     return psw_;
 }

#endif // _KD11_NA_CPU_H_