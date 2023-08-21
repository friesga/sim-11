#ifndef _CCINSTRUCTION_H_
#define _CCINSTRUCTION_H_

#include "../lsi11instruction/lsi11instruction.h"
#include "../kd11cpu.h"

class CcInstruction : public LSI11Instruction
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

public:
	CcInstruction (CpuData* cpu, u16 instruction);
	u16 getOperationCode () override;

protected:
	u16 getConditionCodes ();
};

#endif // !_CCINSTRUCTION_H_
