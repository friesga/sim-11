#ifndef _BLOS_H_
#define _BLOS_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/branchinstruction/branchinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    BLOS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BLOS::BLOS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BLOS::execute ()
{
    executeBranchIf (isSet (PSW_C) || isSet (PSW_Z));
    return CpuData::Trap::None;
}

#endif // _BLOS_H_