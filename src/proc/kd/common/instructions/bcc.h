#ifndef _BCC_H_
#define _BCC_H_


#include "proc/kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "proc/kd/include/cpudata.h"
#include "proc/kd/common/operandlocation/operandlocation.h"


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
class BCC : public BranchInstruction
{
public:
    BCC (u16 instruction);
};

inline BCC::BCC (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BCC_H_