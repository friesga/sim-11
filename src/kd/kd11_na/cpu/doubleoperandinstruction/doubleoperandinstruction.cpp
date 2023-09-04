#include "doubleoperandinstruction.h"

// It would be nice if source and destination operand locations and operands
// could be retrieved in the constructor. This is not possible however, as
// not for all instructions the destination operand has to be retrieved and
// we need a return value for the retrieval.
// 
// Initialize the instr struct with the first member
DoubleOperandInstruction::DoubleOperandInstruction (CpuData* cpu, u16 instruction)
	:
	LSI11Instruction (cpu),
	instr_ {instruction}
{}

OperandLocation DoubleOperandInstruction::getSourceOperandLocation (u16 (&reg)[8])
{
	return decodeOperand (Operand {instr_.decoded.src_rn, instr_.decoded.src_mode}, reg);
}

OperandLocation DoubleOperandInstruction::getDestinationOperandLocation (u16 (&reg)[8])
{
	return decodeOperand (Operand {instr_.decoded.dst_rn, instr_.decoded.dst_mode}, reg);
}

// Return the instruction's operation code
u16 DoubleOperandInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}
