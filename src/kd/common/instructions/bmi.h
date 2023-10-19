#ifndef _BMI_H_
#define _BMI_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::BMI : public BranchInstruction, public WithFactory<BMI>
{
public:
    BMI (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::BMI::BMI (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::BMI::execute ()
{
    executeBranchIf (isSet (PSW_N));
    return CpuData::Trap::None;
}

#endif // _BMI_H_