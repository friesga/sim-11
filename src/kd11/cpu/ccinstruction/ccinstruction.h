#ifndef _CCINSTRUCTION_H_
#define _CCINSTRUCTION_H_

#include "../kd11cpu.h"

class CcInstruction
{
    union instr
	{
		u16 word;
		struct
		{
			u16	conditionCodes:4;
			u16	opcode:12;
		} 
		decoded;
	} 
	instr_;

	KD11CPU *cpu_;

public:
	CcInstruction (u16 instruction);
	u16 getConditionCodes ();
};

#endif // !_CCINSTRUCTION_H_
