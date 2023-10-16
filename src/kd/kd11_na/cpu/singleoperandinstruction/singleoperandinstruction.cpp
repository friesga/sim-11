#include "singleoperandinstruction.h"
#include "../operand/operand.h"

// Initialize the instr struct with the first member
SingleOperandInstruction::SingleOperandInstruction (CpuData *cpu, u16 instruction)
	:
	LSI11Instruction (cpu),
	instr_ {instruction}
{}

// Derive the location of the operand from the addressing mode and the
// register in the instruction and possibly the index and a memory address.
OperandLocation SingleOperandInstruction::getOperandLocation (GeneralRegisters& reg)
{
	return decodeOperand (Operand {instr_.decoded.rn, instr_.decoded.mode}, reg);
}

// Return the instruction's operation code
u16 SingleOperandInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

