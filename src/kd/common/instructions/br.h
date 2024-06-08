#ifndef _BR_H_
#define _BR_H_

#include "commoninstruction.h"
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
class CommonInstruction::BR : public BranchInstruction
{
public:
    BR (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BR::BR (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::BR::execute ()
{
    throw "Should not happen";
}

#endif // _BR_H_