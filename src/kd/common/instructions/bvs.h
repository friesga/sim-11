#ifndef _BVS_H_
#define _BVS_H_


#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class BVS : public BranchInstruction
{
public:
    BVS (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline BVS::BVS (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _BVS_H_