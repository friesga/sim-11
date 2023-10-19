#ifndef _NOOPERANDINSTRUCTION_H_
#define _NOOPERANDINSTRUCTION_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"

class NoOperandInstruction : public LSI11Instruction
{
	u16 instr_;

public:
	NoOperandInstruction (CpuData* cpu, u16 instruction);
	u16 getOperationCode () override;
};

#endif // _NOOPERAND_H_