#include "branchinstruction.h"

BranchInstruction::BranchInstruction (CpuData *cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
    instr_ {instruction}
{}

s8 BranchInstruction::getOffset ()
{
    return instr_.decoded.offset;
}

// Return the instruction's operation code
u16 BranchInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}