#ifndef _KD11_H_
#define _KD11_H_

#include "../qbus/qbus.h"

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
	u16 readW(QBUS* bus, u16 dst, u16 mode, int inc);
	u8 readB(QBUS* bus, u16 dst, u16 mode, int inc);
	void writeW(QBUS* bus, u16 dst, u16 mode, u16 val);
	void writeB(QBUS* bus, u16 dst, u16 mode, u8 val);
	u16 getAddr(QBUS* bus, u16 dst, u16 mode);
	void setTrap(InterruptRequest ir);
	void execInstr(QBUS* bus);

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
