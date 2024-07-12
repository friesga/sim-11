#include "eisinstruction.h"
#include "kd/common/operand/operand.h"

EisInstruction::EisInstruction (u16 instruction)
    :
	LSI11Instruction (),
    instr_ {instruction}
{}

// Return the instruction's operation code
u16 EisInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}

u16 EisInstruction::getOperandRegister () const
{
	return instr_.decoded.operandRegNr;
}

u16 EisInstruction::getOperandMode () const
{
	return instr_.decoded.operandMode;
}

u16 EisInstruction::getRegisterNr () const
{
	return instr_.decoded.registerNr;
}

