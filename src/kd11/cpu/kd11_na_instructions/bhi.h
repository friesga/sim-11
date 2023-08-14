#ifndef _BHI_H_
#define _BHI_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class BHI : public BranchInstruction
    {
    public:
        BHI (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BHI::BHI (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BHI::execute ()
    {
        executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z));
        return CpuData::Trap::None;
    }
}

#endif // _BHI_H_