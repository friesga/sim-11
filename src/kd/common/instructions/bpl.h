#ifndef _BPL_H_
#define _BPL_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::BPL : public BranchInstruction, public WithFactory<BPL>
{
public:
    BPL (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BPL::BPL (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BPL::execute ()
{
    executeBranchIf (!isSet (PSW_N));
    return CpuData::Trap::None;
}

#endif // _BPL_H_