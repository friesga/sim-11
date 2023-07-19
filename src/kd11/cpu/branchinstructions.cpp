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
void KD11CPU::BR (u16 instruction)
{
    BranchInstruction branchInstruction {instruction};

    register_[7] += (s16) branchInstruction.getOffset () * 2;
}

// BNE - branch if not equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z = 0
//
// Tests the state of the Z-bit and causes a branch if the Z-bit is clear.
// BNE is the complementary operation to BEQ. It is used to test inequality
// following a CMP, to test that some bits set in the destination were also
// in the source, following a BIT, and generally, to test that the result of
// the previous operation was not zero.
//
void KD11CPU::BNE (u16 instruction)
{
    if (!PSW_GET (PSW_Z))
    {
        BranchInstruction branchInstruction {instruction};

        register_[7] += (s16) branchInstruction.getOffset () * 2;
    }
}

// BEQ - branch if queal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z = 1
// 
// Tests the state of the Z·bit and causes a branch if Z is set. As an
/// example, it is used to test equality following a CMP operation, to test
// that no bits set in the destination were also set in the source following
// a BIT operation, and generally, to test that the result of the previous
// operation was zero.
//
void KD11CPU::BEQ (u16 instruction)
{
    if (PSW_GET (PSW_Z))
    {
        BranchInstruction branchInstruction {instruction};

        register_[7] += (s16) branchInstruction.getOffset () * 2;
    }
}