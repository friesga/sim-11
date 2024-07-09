#ifndef _BLOS_H_
#define _BLOS_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BLOS : public BranchInstruction
{
public:
    BLOS (u16 instruction);
};

inline BLOS::BLOS (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BLOS_H_