#ifndef _BMI_H_
#define _BMI_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// BMI - branch if minus
//
// Operation:
//  PC <- PC + (2 * offset) if N = 1
//
// Condition Codes: Unaffected
//
// Tests the state of the N-bit and causes a branch if N is set. It is used
// to test the sign (most significant bit) of the result of the previous
// operation), branching if negative. BMI is the complementary function of BPL.
//
class CommonInstruction::BMI : public BranchInstruction
{
public:
    BMI (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BMI::BMI (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BMI::execute ()
{
    executeBranchIf (isSet (PSW_N));
    return true;
}

#endif // _BMI_H_