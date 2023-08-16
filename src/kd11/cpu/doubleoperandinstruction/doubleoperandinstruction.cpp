#include "doubleoperandinstruction.h"

// Initialize the instr struct with the first member
DoubleOperandInstruction::DoubleOperandInstruction (CpuData* cpu, u16 instruction)
	:
	LSI11Instruction (cpu),
	instr_ {instruction}
{}

OperandLocation DoubleOperandInstruction::getSourceOperandLocation (u16 (&reg)[8])
{
	// isByteInstruction (instr_.decoded.opcode)
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

bool DoubleOperandInstruction::getSourceWordOperand ()
{
	OperandLocation sourceOperandLocation = 
		getSourceOperandLocation (cpu_->registers ());
    source_ = sourceOperandLocation.wordContents ();
	return source_.hasValue ();
}

bool DoubleOperandInstruction::getDestinationWordOperand ()
{
	OperandLocation destinationOperandLocation = 
		getDestinationOperandLocation (cpu_->registers ());
    destination_ = destinationOperandLocation.wordContents ();
	return destination_.hasValue ();
}