#ifndef _SINGLEOPERANDINSTRUCTION_H_
#define _SINGLEOPERANDINSTRUCTION_H_

#include "../kd11cpu.h"
#include "../lsi11instruction/lsi11instruction.h"

class SingleOperandInstruction : public LSI11Instruction
{
	// Single operand instruction format comprises the operation code and
	// one operand
	union instr
	{
		u16 word;
		struct
		{
			u16	rn:3;
			u16	mode:3;
			u16	opcode:10;
		} 
		decoded;
	} 
	instr_;

public:
	SingleOperandInstruction (KD11CPU *cpu, u16 instruction);
	OperandLocation getOperandLocation (u16 (&reg)[8]);
	u16 getOperationCode ();
};


#endif // !_SINGLEOPERANDINSTRUCTION_H_
