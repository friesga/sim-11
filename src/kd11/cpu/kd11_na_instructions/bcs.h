#ifndef _BCS_H_
#define _BCS_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BCS - branch if carry is set
//
// Operation:
//  PC <- PC + (2 * offset) if C = 1
//  
// Condition Codes: Unaffected
// 
// Tests the state of the C-bit and causes a branch if C is set. It is used
// to test for a carry in the result of a previous operation.
//
namespace KD11_NA
{
    class BCS : public BranchInstruction
    {
    public:
        BCS (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BCS::BCS (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BCS::execute ()
    {
        executeBranchIf (isSet (PSW_C));
        return CpuData::Trap::None;
    }
}

#endif // _BCS_H_