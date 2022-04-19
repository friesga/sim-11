#ifndef _KD11_H_
#define _KD11_H_

#include "../qbus/qbus.h"

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

	KD11CPU();
	void reset();
	void step(QBUS* bus);
	void handleTraps(QBUS* bus);

	// ToDo: Make runState and r[] private; accessed from main
	u8	runState;
	u16	r[8];

private:
	CondData<u16> readW(QBUS* bus, u16 dst, u16 mode, int inc);
	CondData<u8> readB(QBUS* bus, u16 dst, u16 mode, int inc);
	void writeW(QBUS* bus, u16 dst, u16 mode, u16 val);
	void writeB(QBUS* bus, u16 dst, u16 mode, u8 val);
	CondData<u16> getAddr(QBUS* bus, u16 dst, u16 mode);
	void setTrap(InterruptRequest ir);
	void execInstr(QBUS* bus);
	u8 pswPriority();

	u16	psw;
	InterruptRequest trap;
	InterruptRequest const emptyIntrptReq{TrapPriority::None, 0, 0};
	InterruptRequest const busError{TrapPriority::BusError, 0, 004};
	InterruptRequest const illegalInstructionTrap{TrapPriority::InstructionTrap, 0, 010};
	InterruptRequest const traceTrap{TrapPriority::TraceTrap, 0, 014};
	InterruptRequest const BPT{TrapPriority::InstructionTrap, 0, 014};
	InterruptRequest const IOT{TrapPriority::InstructionTrap, 0, 020};
	InterruptRequest const EMT{TrapPriority::InstructionTrap, 0, 030};
	InterruptRequest const TRP{TrapPriority::InstructionTrap, 0, 034};
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
// ToDo: Make KD11 a QBUSModule
// The module KD11 is now - different from the other QBUSModules - instantiated
// within the LSI11 object, although it is also a QBUS module.
class KD11
{
public:
	void reset();
	void step(QBUS* bus);
	// Give main() access to the CPU to set PC and runState
	KD11CPU &cpu();

private:
	KD11CPU cpu_;
	KD11ODT	odt{cpu_};
};


#endif // !_KD11_H_
