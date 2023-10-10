#ifndef _EISINSTRUCTION_H_
#define _EISINSTRUCTION_H_

#include "../kd11_na_cpu.h"
#include "kd/common/lsi11instruction/lsi11instruction.h"
#include "../operandlocation/operandlocation.h"

// The EIS instruction format is used by the EIS instructions MUL, DIV, ASH
// and ASHC instruction and additionally by the JSR and XOR instructions.
class EisInstruction : public LSI11Instruction
{
	union instr
	{
		u16 word;
		struct
		{
			u16	operandRegNr:3;
			u16	operandMode:3;
			u16	registerNr:3;
			u16	opcode:7;
		} 
		decoded;
	} 
	instr_;

public:
	EisInstruction (CpuData* cpu, u16 instruction);
	u16 getOperationCode () override;
	
protected:
	OperandLocation getOperandLocation (u16 (&reg)[8]);
	bool readOperand (CondData<u16> *source);
	bool writeOperand (u16 operand);
	u16 getRegisterNr ();

private:
	OperandLocation operandLocation_ {};
};


#endif // !_EISOPERANDINSTRUCTION_H_
