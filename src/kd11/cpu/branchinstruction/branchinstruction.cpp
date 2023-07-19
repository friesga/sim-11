#include "branchinstruction.h"

BranchInstruction::BranchInstruction (u16 instruction)
    :
    instr_ {instruction}
{}

s8 BranchInstruction::getOffset ()
{
    return instr_.decoded.offset;
}