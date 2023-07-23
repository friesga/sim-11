#ifndef _DOUBLEINSTRUCTIONFORMAT_H_
#define _DOUBLEINSTRUCTIONFORMAT_H_

#include "../kd11cpu.h"
#include "../operandlocation/operandlocation.h"

class DoubleOperandInstruction
{
	struct Operand
	{
		u16	registerNr:3;
		u16	mode:3;
	};

	// Single operand instruction format comprises the operation code and
	// one operand
	union instr
	{
		u16 word;
		struct
		{
			Operand source;
			Operand destination;
			u16	opcode:4;
		} 
		decoded;
	} 
	instr_;

	KD11CPU *cpu_;

public:
	DoubleOperandInstruction (KD11CPU *cpu, u16 instruction);
	OperandLocation getSourceOperandLocation (u16 (&reg)[8]);
	OperandLocation getDestinationOperandLocation (u16 (&reg)[8]);
};

#endif // _DOUBLEINSTRUCTIONFORMAT_H_