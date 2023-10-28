#ifndef _BR_H_
#define _BR_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
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
    bool execute () override;
};

inline CommonInstruction::BR::BR (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

inline bool CommonInstruction::BR::execute ()
{
    executeBranch ();
    return true;
}

#endif // _BR_H_