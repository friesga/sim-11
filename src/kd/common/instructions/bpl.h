#ifndef _BPL_H_
#define _BPL_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BPL - branch if plus
//
// Operation:
//  PC <- PC + (2 * offset) if N = 0
//
// Condition Codes: Unaffected
//
// Tests the state of the N-bit and causes a branch if N is clear, (positive
// result). BPL is the complementary operation of BMI.
//
class CommonInstruction::BPL : public BranchInstruction
{
public:
    BPL (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BPL::BPL (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::BPL::execute ()
{
    throw "Should not happen";
}

#endif // _BPL_H_