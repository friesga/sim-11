#ifndef _BCC_H_
#define _BCC_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BCC - branch if carry is clear
//
// Operation:
//  PC <- PC + (2 * offset) if C = 0
//  
// Condition Codes: Unaffected
// 
// Tests the state of the C-bit and causes a branch if C is clear. BCC is
// the complementary operation to BCS.
//
namespace KD11_NA
{
    class BCC : public BranchInstruction
    {
    public:
        BCC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BCC::BCC (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BCC::execute ()
    {
        executeBranchIf (!isSet (PSW_C));
        return CpuData::Trap::None;
    }
}

#endif // _BCC_H_