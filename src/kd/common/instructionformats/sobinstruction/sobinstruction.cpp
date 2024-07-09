#include "sobinstruction.h"

SobInstruction::SobInstruction (u16 instruction)
	:
	LSI11Instruction (),
	instr_ {instruction}
{}

u16 SobInstruction::getRegisterNr ()
{
	return instr_.decoded.rn;
}

u16 SobInstruction::getOffset ()
{
	return instr_.decoded.offset;
}

// Return the instruction's operation code
u16 SobInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}