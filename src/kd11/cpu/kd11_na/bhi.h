#ifndef _BHI_H_
#define _BHI_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BHI - branch if higher
//
// Operation:
//  PC <- PC + (2 * offset) if C = 0 and Z = 0
//
// Condition Codes: Unaffected
//
// Causes a branch if the previous operation caused neither a carry nor a zero
// result. This will happen in comparison (CMP) operations as long as the
// source has a higher unsigned value than the destination.
//
class KD11_NAInstruction::BHI : public BranchInstruction, public WithFactory<BHI>
{
public:
    BHI (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BHI::BHI (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BHI::execute ()
{
    executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z));
    return CpuData::Trap::None;
}

#endif // _BHI_H_