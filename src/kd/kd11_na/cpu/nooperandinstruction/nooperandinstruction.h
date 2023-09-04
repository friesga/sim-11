#ifndef _NOOPERANDINSTRUCTION_H_
#define _NOOPERANDINSTRUCTION_H_

#include "../lsi11instruction/lsi11instruction.h"
#include "../kd11_na_cpu.h"
#include "../operandlocation/operandlocation.h"

class NoOperandInstruction : public LSI11Instruction
{
	u16 instr_;

public:
	NoOperandInstruction (CpuData* cpu, u16 instruction);
	u16 getOperationCode () override;
};

#endif // _NOOPERAND_H_