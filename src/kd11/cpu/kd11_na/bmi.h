#ifndef _BMI_H_
#define _BMI_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::BMI : public BranchInstruction, public WithFactory<BMI>
{
public:
    BMI (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::BMI::BMI (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::BMI::execute ()
{
    executeBranchIf (isSet (PSW_N));
    return CpuData::Trap::None;
}

#endif // _BMI_H_