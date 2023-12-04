#ifndef _BCC_H_
#define _BCC_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::BCC : public BranchInstruction, public WithFactory<BCC>
{
public:
    BCC (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BCC::BCC (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BCC::execute ()
{
    executeBranchIf (!isSet (PSW_C));
    return true;
}

#endif // _BCC_H_