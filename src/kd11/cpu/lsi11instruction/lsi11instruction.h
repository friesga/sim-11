#ifndef _LSI11INSTRUCTION_H_
#define _LSI11INSTRUCTION_H_

#include "../operandlocation/operandlocation.h"
#include "../operand/operand.h"

class LSI11Instruction
{
protected:
	KD11CPU *cpu_;

	LSI11Instruction (KD11CPU *cpu);
	OperandLocation decodeOperand (Operand operand, u16 (&reg)[8]);
	bool isByteInstruction (u16 opCode);
};

#endif _LSI11INSTRUCTION_H_