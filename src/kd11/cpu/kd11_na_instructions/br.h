#ifndef _BR_H_
#define _BR_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BR - branch (unconditional)
//
// Operation:
//  PC <- PC + (2 * offset)
//
// Provides a way of transferring program control within a range of -128 (10)
// to +127 (10) words with a one word instruction.
//
namespace KD11_NA
{
    class BR : public BranchInstruction
    {
    public:
        BR (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BR::BR (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BR::execute ()
    {
        executeBranch ();
        return CpuData::Trap::None;
    }
}

#endif // _BR_H_