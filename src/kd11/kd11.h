#ifndef _KD11_H_
#define _KD11_H_

#include "qbus/qbus.h"
#include "busdevice/busdevice.h"
#include "float/float.h"

#include <functional>

#define USE_FLOAT

/* ODT states */
#define	ODT_STATE_INIT		0
#define	ODT_STATE_WAIT		1
#define	ODT_STATE_ADDR		2
#define	ODT_STATE_REG		3
#define	ODT_STATE_REG_WAIT	4
#define	ODT_STATE_VAL		5
#define	ODT_STATE_REG_VAL	6
#define	ODT_STATE_WR		7

/* CPU states */
#define	STATE_HALT		0
#define	STATE_RUN		1
#define	STATE_WAIT		2
#define	STATE_INHIBIT_TRACE	3

// The processor status word in the LSI-11 is a composite of (1) the 4 PDP-11
// Status Flags (N,Z,V,C) [bits 0:3] and (2) the Trace Bit [bit 4] and (3)
// the Interrupt Enable bit [bit 7]. (EK-KUV11-TM-001 pg. 1-9). 
// Processor Status Word bits 5 and 6 are reserved
// (EK-KUV11-TM-001 Figure 2-5). This implies that these bits cannot
// be set by the MTPS instruction and are read as zero by the MFPS
// instruction. This behaviour is confirmed by the VKACC1 FIS diagnostic.
// The diagnostic sets the PSW to the value 040 and then compares the
// PSW's contents with the value 010.

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

class KD11CPU
{
public:
	friend class KD11ODT;

	KD11CPU(QBUS *bus);
	void reset();
	void step(QBUS* bus);
	void handleTraps(QBUS* bus);

	// These functions are defined public as they are used by the
	// Instruction classes.
	// ToDo: fetchWord(), putWord() and putByte() should be private?
	CondData<u16> fetchWord (u16 address);
	bool putWord (u16 address, u16 value);
	bool putByte (u16 address, u8 value);

	// ToDo: Make runState and r[] private; accessed from main
	u8	runState;
	u16	r[8];

private:
	void setTrap(InterruptRequest const *ir);
	void execInstr(QBUS* bus);
	u8 cpuPriority();
	void returnFISresult (Float result, u16 registerNumber);
	void executeFISinstruction (u16 stackPointer, 
		std::function<bool(Float, Float)> argumentsValid,
		std::function<Float(Float, Float)>);

	QBUS *bus_;
	u16	psw;

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
};

// ODT functionality of the KD11
class KD11ODT 
{
public:
	KD11ODT(KD11CPU &cpu);
	void reset();
	void step(QBUS* bus);

private:
	void clear();
	void write(u8 c);
	void writeOctal(u16 val);
	void inputError();

	KD11CPU &cpu_;
	u16	addr;
	u16	val;
	u8	input;
	u8	state;
	u8	next;
	u8	buf[16];
	u8	buf_r;
	u8	buf_sz;
};

// The class KD11 is composed of the KD11 CPU and the KD11 ODT.
// ToDo: Make KD11 a BusDevice
// The module KD11 is now - different from the other QBUSModules - instantiated
// within the LSI11 object, although it is also a QBUS module.
class KD11
{
public:
	KD11 (QBUS *bus);
	void reset();
	void step(QBUS* bus);
	// Give main() access to the CPU to set PC and runState
	KD11CPU &cpu();

private:
	QBUS *bus_;
	KD11CPU cpu_ {bus_};
	KD11ODT	odt {cpu_};
};


#endif // !_KD11_H_
