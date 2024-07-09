#ifndef _BVC_H_
#define _BVC_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BVC : public BranchInstruction
{
public:
    BVC (u16 instruction);
};

inline BVC::BVC (u16 instruction)
    :
    BranchInstruction (instruction)
{}

#endif // _BVC_H_