#ifndef _BR_H_
#define _BR_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/branchinstruction/branchinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// BR - branch (unconditional)
//
// Operation:
//  PC <- PC + (2 * offset)
//
// Provides a way of transferring program control within a range of -128 (10)
// to +127 (10) words with a one word instruction.
//
class CommonInstruction::BR : public BranchInstruction, public WithFactory<BR>
{
public:
    BR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BR::BR (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BR::execute ()
{
    executeBranch ();
    return CpuData::Trap::None;
}

#endif // _BR_H_