#ifndef _SINGLEOPERANDINSTRUCTION_H_
#define _SINGLEOPERANDINSTRUCTION_H_

#include "../kd11cpu.h"
#include "../operandlocation/operandlocation.h"

class SingleOperandInstruction
{
	union instr
	{
		u16 word;
		struct
		{
			u16	registerNr:3;
			u16	mode:3;
			u16	opcode:10;
		} 
		decoded;
	} 
	instr_;

	KD11CPU *cpu_;

public:
	SingleOperandInstruction (KD11CPU *cpu, u16 instruction);
	OperandLocation getOperandLocation (u16 (&reg)[8]);
};


#endif // !_SINGLEOPERANDINSTRUCTION_H_
