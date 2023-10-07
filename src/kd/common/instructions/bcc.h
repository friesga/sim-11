#ifndef _BCC_H_
#define _BCC_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/branchinstruction/branchinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    BCC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BCC::BCC (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BCC::execute ()
{
    executeBranchIf (!isSet (PSW_C));
    return CpuData::Trap::None;
}

#endif // _BCC_H_