#include "doubleoperandinstruction.h"

// It would be nice if source and destination operand locations and operands
// could be retrieved in the constructor. This is not possible however, as
// not for all instructions the destination operand has to be retrieved and
// we need a return value for the retrieval.
// 
// Initialize the instr struct with the first member
DoubleOperandInstruction::DoubleOperandInstruction (CpuData* cpuData, CpuControl* cpuControl,
		MMU* mmu, u16 instruction)
	:
	LSI11Instruction (cpuData, cpuControl, mmu),
	instr_ {instruction}
{}

OperandLocation DoubleOperandInstruction::getSourceOperandLocation (GeneralRegisters &reg)
{
	return decodeOperand (Operand {instr_.decoded.src_rn, instr_.decoded.src_mode}, reg);
}

OperandLocation DoubleOperandInstruction::getDestinationOperandLocation (GeneralRegisters &reg)
{
	return decodeOperand (Operand {instr_.decoded.dst_rn, instr_.decoded.dst_mode}, reg);
}

// On most PDP-11's the source operand location is determined before the
// destination operand location. This is also the case for the KD11-NA.
void DoubleOperandInstruction::getOperandLocations (GeneralRegisters &reg)
{
	sourceOperandLocation_ = getSourceOperandLocation (reg);
	destinationOperandLocation_ = getDestinationOperandLocation (reg);
}

// Return the instruction's operation code
u16 DoubleOperandInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}
