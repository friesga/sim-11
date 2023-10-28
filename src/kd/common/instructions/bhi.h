#ifndef _BHI_H_
#define _BHI_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
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
class CommonInstruction::BHI : public BranchInstruction, public WithFactory<BHI>
{
public:
    BHI (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BHI::BHI (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

inline bool CommonInstruction::BHI::execute ()
{
    executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z));
    return true;
}

#endif // _BHI_H_