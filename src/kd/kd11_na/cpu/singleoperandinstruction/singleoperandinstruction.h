#ifndef _SINGLEOPERANDINSTRUCTION_H_
#define _SINGLEOPERANDINSTRUCTION_H_

#include "../kd11_na_cpu.h"
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
	template <typename T> bool readOperand (T *operand);
	template <typename T> bool writeOperand (T operand);
};

// The functions below are templated for bytes (type u8 or CondData<u8>) and
// words (type u16 and CondData<u16>). Trying to use the functions for other
// types will result in compilation errors.
template <typename T>
bool SingleOperandInstruction::readOperand (T *operand)
{
	operandLocation_ =  getOperandLocation (cpu_->registers ());
    *operand = operandLocation_.contents<T> ();
	return (*operand).hasValue ();
}

template <typename T>
bool SingleOperandInstruction::writeOperand (T operand)
{
	if (!operandLocation_.isValid ())
	{
		operandLocation_ = 
			getOperandLocation (cpu_->registers ());
	}
	
	return operandLocation_.write<T> (operand);
}

#endif // !_SINGLEOPERANDINSTRUCTION_H_
