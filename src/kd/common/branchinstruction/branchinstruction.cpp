#include "branchinstruction.h"

BranchInstruction::BranchInstruction (CpuData *cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    LSI11Instruction (cpu, cpuControl, mmu),
    instr_ {instruction}
{}

void BranchInstruction::executeBranch ()
{
    cpu_->registers ()[7] += (s16) getOffset () * 2;
}

s8 BranchInstruction::getOffset ()
{
    return instr_.decoded.offset;
}

// Return the instruction's operation code
u16 BranchInstruction::getOperationCode ()
{
	return instr_.decoded.opcode;
}