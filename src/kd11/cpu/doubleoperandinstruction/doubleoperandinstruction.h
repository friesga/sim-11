#ifndef _DOUBLEINSTRUCTIONFORMAT_H_
#define _DOUBLEINSTRUCTIONFORMAT_H_

#include "../kd11cpu.h"
#include "../lsi11instruction/lsi11instruction.h"

class DoubleOperandInstruction : public LSI11Instruction
{
	// The double operand instruction format comprises the operation code, the
	// source operand and the destination operand.
	// 
	union instr
	{
		u16 word;
		struct
		{
			u16	dst_rn:3;
			u16	dst_mode:3;
			u16	src_rn:3;
			u16	src_mode:3;
			u16	opcode:4;
		} 
		decoded;
	} 
	instr_;

public:
	DoubleOperandInstruction (CpuData* cpu, u16 instruction);
	OperandLocation getSourceOperandLocation (u16 (&reg)[8]);
	OperandLocation getDestinationOperandLocation (u16 (&reg)[8]);
	u16 getOperationCode ();
};

#endif // _DOUBLEINSTRUCTIONFORMAT_H_