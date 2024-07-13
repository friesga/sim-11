#include "branchinstruction.h"

BranchInstruction::BranchInstruction (u16 instruction)
    :
    LSI11Instruction (),
    instr_ {instruction}
{}

s8 BranchInstruction::getOffset () const
{
    return instr_.decoded.offset;
}

// Return the instruction's operation code
u16 BranchInstruction::getOperationCode () const
{
	return instr_.decoded.opcode;
}