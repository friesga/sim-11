#ifndef _BLOS_H_
#define _BLOS_H_

#include "kd11_nainstruction.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NAInstruction::BLOS : public BranchInstruction, public WithFactory<BLOS>
{
public:
    BLOS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BLOS::BLOS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BLOS::execute ()
{
    executeBranchIf (isSet (PSW_C) || isSet (PSW_Z));
    return CpuData::Trap::None;
}

#endif // _BLOS_H_