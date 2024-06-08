#ifndef _MARKINSTRUCTION_H_
#define _MARKINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "kd/common/operandlocation/operandlocation.h"

class MarkInstruction : public LSI11Instruction
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

public:
	MarkInstruction (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
	u16 getOperationCode () override;

	u16 numberOfParameters ();
};

#endif // _MARKINSTRUCTION_H_