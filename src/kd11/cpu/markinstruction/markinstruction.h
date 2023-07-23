#ifndef _MARKINSTRUCTION_H_
#define _MARKINSTRUCTION_H_

#include "../kd11cpu.h"
#include "../operandlocation/operandlocation.h"

class MarkInstruction
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

	KD11CPU *cpu_;

public:
	MarkInstruction (KD11CPU *cpu, u16 instruction);
	u16 numberOfParameters ();
};

#endif // _MARKINSTRUCTION_H_