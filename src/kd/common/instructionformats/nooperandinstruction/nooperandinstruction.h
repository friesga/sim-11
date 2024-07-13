#ifndef _NOOPERANDINSTRUCTION_H_
#define _NOOPERANDINSTRUCTION_H_

#include "kd/common/instructionformats/pdp111instruction/pdp11instruction.h"
#include "kd/common/operandlocation/operandlocation.h"

class NoOperandInstruction : public PDP11Instruction
{
	u16 instr_;

public:
	NoOperandInstruction (u16 instruction);
	u16 getOperationCode () const override;
};

#endif // _NOOPERAND_H_