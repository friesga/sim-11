#ifndef _SOBINSTRUCTION_H_
#define _SOBINSTRUCTION_H_

#include "kd/common/instructionformats/pdp111instruction/pdp11instruction.h"

class SobInstruction : public PDP11Instruction
{
    // The SOB instruction format comprises the operation code, the register
	// and an offset.
	union instr
	{
		u16 word;
		struct
		{
			u16	offset:6;
			u16	rn:3;
			u16	opcode:7;
		} 
		decoded;
	} 
	instr_;

public:
	SobInstruction (u16 instruction);
	u16 getRegisterNr () const;
	u16 getOffset () const;
	u16 getOperationCode () const;
};


#endif // _SOBINSTRUCTION_H_