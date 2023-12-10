#ifndef _BGT_H_
#define _BGT_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BGT - branch if greater than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 0
//
// Operation of BGT is similar to BGE, except BGT will not cause a branch on
// a zero result.
class CommonInstruction::BGT : public BranchInstruction, public WithFactory<BGT>
{
public:
    BGT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BGT::BGT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BGT::execute ()
{
    executeBranchIf ((isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V))) == 0);
    return true;
}

#endif // _BGT_H_