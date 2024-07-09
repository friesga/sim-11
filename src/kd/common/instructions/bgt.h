#ifndef _BGT_H_
#define _BGT_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BGT - branch if greater than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 0
//
// Operation of BGT is similar to BGE, except BGT will not cause a branch on
// a zero result.
class BGT : public BranchInstruction
{
public:
    BGT (u16 instruction);
};

inline BGT::BGT (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BGT_H_