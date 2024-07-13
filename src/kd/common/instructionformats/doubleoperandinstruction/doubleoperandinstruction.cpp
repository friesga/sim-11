#include "doubleoperandinstruction.h"

// It would be nice if source and destination operand locations and operands
// could be retrieved in the constructor. This is not possible however, as
// not for all instructions the destination operand has to be retrieved and
// we need a return value for the retrieval.
// 
// Initialize the instr struct with the first member
DoubleOperandInstruction::DoubleOperandInstruction (u16 instruction)
	:
	LSI11Instruction (),
	instr_ {instruction}
{}


// Return the instruction's operation code
u16 DoubleOperandInstruction::getOperationCode () const
{
	return instr_.decoded.opcode;
}

u16 DoubleOperandInstruction::getSourceRegister () const
{
	return instr_.decoded.src_rn;
}

u16 DoubleOperandInstruction::getSourceMode () const
{
	return instr_.decoded.src_mode;
}

u16 DoubleOperandInstruction::getDestinationRegister () const
{
	return instr_.decoded.dst_rn;
}

u16 DoubleOperandInstruction::getDestinationMode () const
{
	return instr_.decoded.dst_mode;
}
