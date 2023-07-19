#include "branchinstruction/branchinstruction.h"
#include "kd11cpu.h"

// BR - branch (unconditional)
//
// Operation:
//  PC <- PC + (2 * offset)
//
// Provides a way of transferring program control within a range of -128 (10)
// to +127 (10) words with a one word instruction.
//
void KD11CPU::BR (KD11CPU *cpu, u16 instruction)
{
    BranchInstruction branchInstruction {instruction};

    cpu->register_[7] += (s16) branchInstruction.getOffset () * 2;
}