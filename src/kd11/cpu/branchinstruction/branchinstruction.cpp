#include "branchinstruction.h"

namespace KD11_NA { class BR; }

BranchInstruction::BranchInstruction (CpuData *cpu, u16 instruction)
    :
    LSI11Instruction (cpu),
    instr_ {instruction}
{}

void BranchInstruction::executeBranch ()
{
    cpu_->registers ()[7] += (s16) getOffset () * 2;
}

// Execute the branch given in the instruction under the given condition.
constexpr void BranchInstruction::executeBranchIf (bool condition)
{
    if (condition)
        executeBranch ();
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