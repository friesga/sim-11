#ifndef _BVC_H_
#define _BVC_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    BVC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::BVC::BVC (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::BVC::execute ()
{
    executeBranchIf (!isSet (PSW_V));
    return CpuData::Trap::None;
}

#endif // _BVC_H_