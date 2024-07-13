#ifndef _FISINSTRUCTION_H_
#define _FISINSTRUCTION_H_

#include "kd/common/instructionformats/pdp111instruction/pdp11instruction.h"
#include "float/float.h"

#include <functional>

// The FIS instruction format is used for the FIS instructions, FADD, FSUB,
// FMUL and FDIV. The RTS instruction uses the same format.
class FISInstruction : public PDP11Instruction
{
    union instr
	{
		u16 word;
		struct
		{
			u16	rn:3;
			u16	opcode:13;
		} 
		decoded;
	} 
	instr_;

public:
	FISInstruction (u16 instruction);
	u16 getRegister () const;
	u16 getOperationCode () const override;
};

#endif // _FISINSTRUCTION_H_