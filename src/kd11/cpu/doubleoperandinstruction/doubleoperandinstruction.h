#ifndef _DOUBLEINSTRUCTIONFORMAT_H_
#define _DOUBLEINSTRUCTIONFORMAT_H_

#include "../kd11cpu.h"
#include "../lsi11instruction/lsi11instruction.h"

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
	DoubleOperandInstruction (CpuData* cpu, u16 instruction);

	// ToDo: Make getSourceOperandLocation and getDestinationOperandLocation private
	OperandLocation getSourceOperandLocation (u16 (&reg)[8]);
	OperandLocation getDestinationOperandLocation (u16 (&reg)[8]);
	u16 getOperationCode ();

protected:
	bool readSourceOperand (CondData<u16> *source);
	bool readSourceOperand (CondData<u8> *source);
	bool readDestinationWordOperand (CondData<u16> *destination);
	bool writeDestinationWordOperand (u16 operand);
	bool writeDestinationByteOperand (u8 operand);

private:
	// The destination operand location is defined as a class memeber as for
	// some instructions, such as the ADD instruction, that location has to
	// be used to retrieve and to write the destination operand.
	OperandLocation destinationOperandLocation_ {};
};

#endif // _DOUBLEINSTRUCTIONFORMAT_H_