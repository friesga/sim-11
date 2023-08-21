#ifndef _SINGLEOPERANDINSTRUCTION_H_
#define _SINGLEOPERANDINSTRUCTION_H_

#include "../kd11cpu.h"
#include "../lsi11instruction/lsi11instruction.h"

class SingleOperandInstruction : public LSI11Instruction
{
	// Single operand instruction format comprises the operation code and
	// one operand
	union instr
	{
		u16 word;
		struct
		{
			u16	rn:3;
			u16	mode:3;
			u16	opcode:10;
		} 
		decoded;
	} 
	instr_;

public:
	SingleOperandInstruction (CpuData *cpu, u16 instruction);
	u16 getOperationCode () override;

protected:
	// The operand location is protected as some instructions (notably the JMP
	// and MFPS instructions) are special cases and need access to the location.
	OperandLocation operandLocation_ {};

	OperandLocation getOperandLocation (u16 (&reg)[8]);
	bool readOperand (CondData<u16> *operand);
	bool readOperand (CondData<u8> *operand);
	bool writeOperand (u16 operand);
	bool writeOperand (u8 operand);

};


#endif // !_SINGLEOPERANDINSTRUCTION_H_
