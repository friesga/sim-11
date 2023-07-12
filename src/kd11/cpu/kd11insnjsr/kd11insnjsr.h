#ifndef _KD11INSNJSR_H_
#define _KD11INSNJSR_H_

#include "../instruction/instruction.h"

// (Try to) determine the byte order. To that end gcc provides the __BYTE__ORDER__
// definition. Msvc has no such definition but we can safely assume that all
// win32 machines are little endian.
#if _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
// Little endian host

struct KD11INSNJSR : public Instruction
{
	u16	rn:3;
	u16	mode:3;
	u16	r:3;
	u16	opcode:7;

	bool getOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue);
	bool getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue);
	bool putOperand (KD11CPU *cpu, u16 (&reg)[8], u16 val);
};

#else

struct KD11INSNJSR
{
	u16	opcode:7;
	u16	register_:3;
	u16	mode:3;
	u16	rn:3;
};

#endif // _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#endif // !_KD11INSNJSR_H_
