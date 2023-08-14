#ifndef _BVC_H_
#define _BVC_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BVC - branch if overflow is clear
//
// Operation:
//  PC <- PC + (2 * offset) if V = 0
// 
// Condition Codes: Unaffected
//
// Tests the state of the V bit and causes a branch if the V bit is clear.
// BVC is complementary operation to BVS.
//
namespace KD11_NA
{
    class BVC : public BranchInstruction
    {
    public:
        BVC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BVC::BVC (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BVC::execute ()
    {
        executeBranchIf (!isSet (PSW_V));
        return CpuData::Trap::None;
    }
}

#endif // _BVC_H_