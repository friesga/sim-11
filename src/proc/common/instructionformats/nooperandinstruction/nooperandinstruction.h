#ifndef _NOOPERANDINSTRUCTION_H_
#define _NOOPERANDINSTRUCTION_H_

#include "proc/common/instructionformats/pdp11instruction/pdp11instruction.h"
#include "proc/kd/common/operandlocation/operandlocation.h"

class NoOperandInstruction : public PDP11Instruction
{
	u16 instr_;

public:
	NoOperandInstruction (u16 instruction);
	u16 getOperationCode () const override;
};

#endif // _NOOPERAND_H_