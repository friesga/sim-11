#ifndef _KD11INS1_H_
#define _KD11INS1_H_

#include "../instruction/instruction.h"

// (Try to) determine the byte order. To that end gcc provides the __BYTE__ORDER__
// definition. Msvc has no such definition but we can safely assume that all
// win32 machines are little endian.
#if _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
// Little endian host

// Instruction format for single operand instructions
// 
// For auto-increment and auto-decrement instructions, the increment or
// decrement is performed in the last function retrieving the operand.
// Therefore there are two versions of the getOperand() instructions,
// one without auto-inc/decrement and one performing the increment/
// decrement.
struct KD11INSN1 : public Instruction
{
	u16	rn:3;
	u16	mode:3;
	u16	opcode:10;

	bool getOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 &retValue);
	bool getAddress (KD11CPU *cpu, u16 (&reg)[8], u16 &retValue);
	bool putOperand (KD11CPU *cpu, u16 (&reg)[8], 
		Bitmask<OperandOptions> options, u16 val);
};


#else

/* big endian host */
// ToDo: Add operand instructions to the KD11INS structs
struct KD11INSN1
{
	u16	opcode:10;
	u16	mode:3;
	u16	rn:3;
};

#endif // _WIN32 || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#endif // _KD11NS1_H_