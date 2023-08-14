#ifndef _BLOS_H_
#define _BLOS_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class BLOS : public BranchInstruction
    {
    public:
        BLOS (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BLOS::BLOS (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BLOS::execute ()
    {
        executeBranchIf (isSet (PSW_C) || isSet (PSW_Z));
        return CpuData::Trap::None;
    }
}

#endif // _BLOS_H_