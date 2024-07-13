#include "singleoperandinstruction.h"
#include "kd/common/operand/operand.h"

// Initialize the instr struct with the first member
SingleOperandInstruction::SingleOperandInstruction (u16 instruction)
	:
	instr_ {instruction}
{}

u16 SingleOperandInstruction::getRegister () const
{
	return instr_.decoded.rn;
}

u16 SingleOperandInstruction::getMode () const
{
	return instr_.decoded.mode;
}

// Return the instruction's operation code
u16 SingleOperandInstruction::getOperationCode () const
{
	return instr_.decoded.opcode;
}

