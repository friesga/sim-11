#include "branchinstruction/branchinstruction.h"
#include "kd11cpu.h"

// Execute the branch given in the instruction under the given condition.
constexpr void KD11CPU::executeBranchIf (bool condition, 
    CpuData* cpu, u16 instruction)
{
    if (condition)
        BR (cpu, instruction);
}


// BR - branch (unconditional)
//
// Operation:
//  PC <- PC + (2 * offset)
//
// Provides a way of transferring program control within a range of -128 (10)
// to +127 (10) words with a one word instruction.
//
void KD11CPU::BR (CpuData* cpu, u16 instruction)
{
    BranchInstruction branchInstruction {cpu, instruction};

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
void KD11CPU::BNE (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (!isSet (PSW_Z), cpu, instruction);
}

// BEQ - branch if queal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z = 1
// 
// Tests the state of the Z·bit and causes a branch if Z is set. As an
// example, it is used to test equality following a CMP operation, to test
// that no bits set in the destination were also set in the source following
// a BIT operation, and generally, to test that the result of the previous
// operation was zero.
//
void KD11CPU::BEQ (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_Z), cpu, instruction);
}

// BGE - branch if greater than or equal (to zero)
// 
// Operation:
//  PC <- PC + (2 * offset) if N xor V = 0
//
// Causes a branch if N and V are either both clear or both set. BGE is the
// complementary operation to BLT. Thus BGE will always cause a branch when it
// follows an operation that caused addition of two positive numbers. BGE will
// also cause a branch on a zero result.
//
void KD11CPU::BGE (CpuData*  cpu, u16 instruction)
{
    executeBranchIf ((isSet (PSW_N) ^ isSet (PSW_V)) == 0, cpu, instruction);
}

// BLT - branch if less than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if N xor V = 1
//
// Causes a branch if the "Exclusive Or" of the N and V bits are 1. Thus BLT
// will always branch following an operation that added two negative numbers,
// even if overflow occurred. In particular, BLT will always cause a branch if
// it follows a CMP instruction operating on a negative source and a positive
// destination (even if overflow occurred). Further. BLT will never cause a
// branch when it follows a CMP instruction operating on a positive source and
// negative destination. BLT will not cause a branch if the result of the
// previous operation was zero (without overflow).
//
void KD11CPU::BLT (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_N) ^ isSet (PSW_V), cpu, instruction);
}

// BGT - branch if greater than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 0
//
// Operation of BGT is similar to BGE, except BGT will not cause a branch on
// a zero result.
void KD11CPU::BGT (CpuData*  cpu, u16 instruction)
{
    executeBranchIf ((isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V))) == 0,
        cpu, instruction);
}

// BLE - branch if less than or equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 1
//
// Operation is similar to BLT but in addition will cause a branch if the
// result of the previous operation was zero.
//
void KD11CPU::BLE (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V)),
        cpu, instruction);
}

// BPL - branch if plus
//
// Operation:
//  PC <- PC + (2 * offset) if N = 0
//
// Condition Codes: Unaffected
//
// Tests the state of the N-bit and causes a branch if N is clear, (positive
// result). BPL is the complementary operation of BMI.
//
void KD11CPU::BPL (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (!isSet (PSW_N), cpu, instruction);
}

// BMI - branch if minus
//
// Operation:
//  PC <- PC + (2 * offset) if N = 1
//
// Condition Codes: Unaffected
//
// Tests the state of the N-bit and causes a branch if N is set. It is used
// to test the sign (most significant bit) of the result of the previous
// operation), branching if negative. BMI is the complementary function of BPL.
//
void KD11CPU::BMI (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_N), cpu, instruction);
}

// BHI - branch if higher
//
// Operation:
//  PC <- PC + (2 * offset) if C = 0 and Z = 0
//
// Condition Codes: Unaffected
//
// Causes a branch if the previous operation caused neither a carry nor a zero
// result. This will happen in comparison (CMP) operations as long as the
// source has a higher unsigned value than the destination.
//
void KD11CPU::BHI (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z), cpu, instruction);
}

// BLOS - branch if lower or same
//
// Operation:
//  PC <- PC + (2 * offset) if C v Z = 1
//
// Condition Codes: Unaffected
//
// Causes a branch if the previous operation caused either a carry or a zero
// result. BLOS is the complementary operation to BHI. The branch will occur
// in comparison operations as long as the source is equal to, or has a lower
// unsigned value than the destination.
//
void KD11CPU::BLOS (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_C) || isSet (PSW_Z), cpu, instruction);
}

// BVC - branch if overflow is clear
//
// Operation:
//  PC <- PC + (2 * offset) if V = 0
// 
// Condition Codes: Unaffected
//
// Tests the state of the V bit and causes a branch if the V bit is clear.
// BVC is complementary operation to BVS.
//
void KD11CPU::BVC (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (!isSet (PSW_V), cpu, instruction);
}

// BVS - branch if overflow is set
//
// Operation:
// PC <- PC + (2 * offset) if V = 1
//  
// Condition Codes: Unaffected
//
// Tests the state of V bit (overflow) and causes a branch if the V bit is
// set. BVS is used to detect arithmetic overflow in the previous operation.
//
void KD11CPU::BVS (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_V), cpu, instruction);
}

// BCC - branch if carry is clear
//
// Operation:
//  PC <- PC + (2 * offset) if C = 0
//  
// Condition Codes: Unaffected
// 
// Tests the state of the C-bit and causes a branch if C is clear. BCC is
// the complementary operation to BCS.
//
void KD11CPU::BCC (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (!isSet (PSW_C), cpu, instruction);
}

// BCS - branch if carry is set
//
// Operation:
//  PC <- PC + (2 * offset) if C = 1
//  
// Condition Codes: Unaffected
// 
// Tests the state of the C-bit and causes a branch if C is set. It is used
// to test for a carry in the result of a previous operation.
//
void KD11CPU::BCS (CpuData*  cpu, u16 instruction)
{
    executeBranchIf (isSet (PSW_C), cpu, instruction);
}