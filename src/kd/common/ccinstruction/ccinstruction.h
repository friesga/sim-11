#ifndef _CCINSTRUCTION_H_
#define _CCINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"

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
	CcInstruction (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
	u16 getOperationCode () override;

protected:
	u16 getConditionCodes ();
};

#endif // !_CCINSTRUCTION_H_
