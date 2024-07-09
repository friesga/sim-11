#ifndef _BHI_H_
#define _BHI_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BHI : public BranchInstruction
{
public:
    BHI (u16 instruction);
};

inline BHI::BHI (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BHI_H_