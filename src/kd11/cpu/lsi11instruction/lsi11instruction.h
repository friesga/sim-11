#ifndef _LSI11INSTRUCTION_H_
#define _LSI11INSTRUCTION_H_

#include "../operandlocation/operandlocation.h"

class LSI11Instruction
{
protected:
    struct Operand
	{
		u16	registerNr:3;
		u16	mode:3;
	};

	KD11CPU *cpu_;

	LSI11Instruction (KD11CPU *cpu);
	OperandLocation decodeOperand (Operand operand, u16 (&reg)[8]);
};

#endif _LSI11INSTRUCTION_H_