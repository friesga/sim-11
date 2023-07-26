#ifndef _ARITHMETICINSTRUCTION_H_
#define _ARITHMETICINSTRUCTION_H_

#include "../kd11cpu.h"

// The Fixed and Floating Point Arithmetic instruction format is used for
// the EIS and FIS instructions, FADD, FSUB, FMUL, FDIV, MUL, DIV, ASH and
// ASHC). The RTS instruction uses the same format.
class ArithmeticInstruction
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
	ArithmeticInstruction (KD11CPU *cpu, u16 instruction);
	u16 getRegister ();
};

#endif // _ARITHMETICINSTRUCTION_H_