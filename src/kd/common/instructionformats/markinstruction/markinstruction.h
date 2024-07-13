#ifndef _MARKINSTRUCTION_H_
#define _MARKINSTRUCTION_H_

#include "kd/common/instructionformats/pdp111instruction/pdp11instruction.h"
#include "kd/common/operandlocation/operandlocation.h"

class MarkInstruction : public PDP11Instruction
{
    union instr
	{
		u16 word;
		struct
		{
			u16	nn:6;
			u16	opcode:10;
		} 
		decoded;
	} 
	instr_;

public:
	MarkInstruction (u16 instruction);
	u16 getOperationCode () const override;

	u16 numberOfParameters ();
};

#endif // _MARKINSTRUCTION_H_