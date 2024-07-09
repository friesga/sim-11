#ifndef _BCS_H_
#define _BCS_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BCS : public BranchInstruction
{
public:
    BCS (u16 instruction);
};

inline BCS::BCS (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BCS_H_