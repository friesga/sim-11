#ifndef _DOUBLEINSTRUCTIONFORMAT_H_
#define _DOUBLEINSTRUCTIONFORMAT_H_

#include "kd/common/lsi11instruction/lsi11instruction.h"

class DoubleOperandInstruction : public LSI11Instruction
{
	// The double operand instruction format comprises the operation code, the
	// source operand and the destination operand.
	// 
	union instr
	{
		u16 word;
		struct
		{
			u16	dst_rn:3;
			u16	dst_mode:3;
			u16	src_rn:3;
			u16	src_mode:3;
			u16	opcode:4;
		} 
		decoded;
	} 
	instr_;

public:
	DoubleOperandInstruction (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
	u16 getOperationCode ();

protected:
	// The destination operand location is defined protected as the MOVB
	// instruction needs to know its type.
	OperandLocation destinationOperandLocation_ {};

	OperandLocation getSourceOperandLocation (GeneralRegisters &reg);
	OperandLocation getDestinationOperandLocation (GeneralRegisters &reg);
	void getOperandLocations (GeneralRegisters &reg);
	template <typename T> bool readSourceOperand (T *source);
	template <typename T> bool readDestinationOperand (T *destination);
	template <typename T> bool writeDestinationOperand (T operand);

private:
	// The source and destination operand locations are defined as class
	// members as for the KD11-F in some cases the destination operand
	// location has to be determined before the source operand is retrieved.
	OperandLocation sourceOperandLocation_ {};
};

// The functions below are templated for bytes (type u8 or CondData<u8>) and
// words (type u16 and CondData<u16>). Trying to use the functions for other
// types will result in compilation errors.
template <typename T>
bool DoubleOperandInstruction::readSourceOperand (T *source)
{
	if (!sourceOperandLocation_.isValid ())
	{
		sourceOperandLocation_ = 
			getSourceOperandLocation (cpu_->registers ());
	}

    *source = sourceOperandLocation_.contents<T> ();
	return (*source).hasValue ();
}

template <typename T>
bool DoubleOperandInstruction::readDestinationOperand (T *destination)
{
	if (!destinationOperandLocation_.isValid ()) 
	{
		destinationOperandLocation_ = 
			getDestinationOperandLocation (cpu_->registers ());
	}

    *destination = destinationOperandLocation_.contents<T> ();
	return (*destination).hasValue ();
}

// For most instructions the destination operand location will have been
// determined when the destion operand has been retrieved. Some instructions
// however just write the destination operand. In these cases the operand
// location still has to be determined.
template <typename T>
bool DoubleOperandInstruction::writeDestinationOperand (T operand)
{
	if (!destinationOperandLocation_.isValid ())
	{
		destinationOperandLocation_ = 
			getDestinationOperandLocation (cpu_->registers ());
	}
	
	return destinationOperandLocation_.write<T> (operand);
}

#endif // _DOUBLEINSTRUCTIONFORMAT_H_