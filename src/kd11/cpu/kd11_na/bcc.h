#ifndef _BCC_H_
#define _BCC_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::BCC : public BranchInstruction, public WithFactory<BCC>
{
public:
    BCC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::BCC::BCC (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::BCC::execute ()
{
    executeBranchIf (!isSet (PSW_C));
    return CpuData::Trap::None;
}

#endif // _BCC_H_