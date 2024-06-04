#ifndef _BLOS_H_
#define _BLOS_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::BLOS : public BranchInstruction, public WithFactory<BLOS>
{
public:
    BLOS (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BLOS::BLOS (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BLOS::execute ()
{
    executeBranchIf (isSet (PSW_C) || isSet (PSW_Z));
    return true;
}

#endif // _BLOS_H_