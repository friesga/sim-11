#ifndef _BPL_H_
#define _BPL_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::BPL : public BranchInstruction, public WithFactory<BPL>
{
public:
    BPL (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::BPL::BPL (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::BPL::execute ()
{
    executeBranchIf (!isSet (PSW_N));
    return CpuData::Trap::None;
}

#endif // _BPL_H_