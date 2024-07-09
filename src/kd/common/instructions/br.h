#ifndef _BR_H_
#define _BR_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BR - branch (unconditional)
//
// Operation:
//  PC <- PC + (2 * offset)
//
// Provides a way of transferring program control within a range of -128 (10)
// to +127 (10) words with a one word instruction.
//
class BR : public BranchInstruction
{
public:
    BR (u16 instruction);
};

inline BR::BR (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BR_H_