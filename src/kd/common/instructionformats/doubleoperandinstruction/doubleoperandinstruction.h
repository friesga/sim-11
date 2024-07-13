#ifndef _DOUBLEINSTRUCTIONFORMAT_H_
#define _DOUBLEINSTRUCTIONFORMAT_H_

#include "kd/common/instructionformats/pdp111instruction/pdp11instruction.h"

class DoubleOperandInstruction : public PDP11Instruction
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
	DoubleOperandInstruction (u16 instruction);
	u16 getOperationCode () const;
	u16 getSourceRegister () const;
	u16 getSourceMode () const;
	u16 getDestinationRegister () const;
	u16 getDestinationMode () const;
};

#endif // _DOUBLEINSTRUCTIONFORMAT_H_