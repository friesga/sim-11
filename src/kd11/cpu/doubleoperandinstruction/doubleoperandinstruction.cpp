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

bool DoubleOperandInstruction::readSourceWordOperand ()
{
	OperandLocation sourceOperandLocation = 
		getSourceOperandLocation (cpu_->registers ());
    source_ = sourceOperandLocation.wordContents ();
	return source_.hasValue ();
}

bool DoubleOperandInstruction::readDestinationWordOperand ()
{
	destinationOperandLocation_ = 
		getDestinationOperandLocation (cpu_->registers ());
    destination_ = destinationOperandLocation_.wordContents ();
	return destination_.hasValue ();
}

// For most instructions the destination operand location will have be
// determined when the destion operand has been retrieved. Some instructions
// however just write the destination operand. In these cases the operand
// location still has to be determined.
bool DoubleOperandInstruction::writeDestinationWordOperand (u16 operand)
{
	if (!destinationOperandLocation_.isValid ())
	{
		destinationOperandLocation_ = 
			getDestinationOperandLocation (cpu_->registers ());
	}
	
	return destinationOperandLocation_.write (operand);
}