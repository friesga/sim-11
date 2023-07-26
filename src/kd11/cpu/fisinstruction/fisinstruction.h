#ifndef _FISINSTRUCTION_H_
#define _FISINSTRUCTION_H_

#include "../kd11cpu.h"

// The FIS instruction format is used for the FIS instructions, FADD, FSUB,
// FMUL and FDIV. The RTS instruction uses the same format.
class FISInstruction
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

	KD11CPU *cpu_;

public:
	FISInstruction (KD11CPU *cpu, u16 instruction);
	u16 getRegister ();
};

#endif // _FISINSTRUCTION_H_