#include "markinstruction.h"

MarkInstruction::MarkInstruction (u16 instruction)
    :
    PDP11Instruction (),
    instr_ {instruction}
{}

u16 MarkInstruction::numberOfParameters ()
{
    return instr_.decoded.nn;
}

// Return the instruction's operation code
u16 MarkInstruction::getOperationCode () const
{
	return instr_.decoded.opcode;
}