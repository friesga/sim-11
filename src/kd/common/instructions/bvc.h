#ifndef _BVC_H_
#define _BVC_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::BVC : public BranchInstruction, public WithFactory<BVC>
{
public:
    BVC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BVC::BVC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BVC::execute ()
{
    executeBranchIf (!isSet (PSW_V));
    return true;
}

#endif // _BVC_H_