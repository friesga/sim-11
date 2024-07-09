#ifndef _BPL_H_
#define _BPL_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BPL : public BranchInstruction
{
public:
    BPL (u16 instruction);
};

inline BPL::BPL (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BPL_H_