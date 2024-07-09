#ifndef _BGE_H_
#define _BGE_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BGE : public BranchInstruction
{
public:
    BGE (u16 instruction);
};

inline BGE::BGE (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BGE_H_