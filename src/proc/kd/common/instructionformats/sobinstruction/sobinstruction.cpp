#include "sobinstruction.h"

SobInstruction::SobInstruction (u16 instruction)
	:
	instr_ {instruction}
{}

u16 SobInstruction::getRegisterNr () const
{
	return instr_.decoded.rn;
}

u16 SobInstruction::getOffset () const
{
	return instr_.decoded.offset;
}

// Return the instruction's operation code
u16 SobInstruction::getOperationCode () const
{
	return instr_.decoded.opcode;
}