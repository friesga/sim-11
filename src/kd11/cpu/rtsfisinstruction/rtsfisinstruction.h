#ifndef _RTSFISINSTRUCTION_H_
#define _RTSFISINSTRUCTION_H_

#include "../kd11cpu.h"

class RtsFisInstruction
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
	RtsFisInstruction (KD11CPU *cpu, u16 instruction);
	u16 getRegister ();
};

#endif // _RTSFISINSTRUCTION_H_