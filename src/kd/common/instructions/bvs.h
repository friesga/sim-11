#ifndef _BVS_H_
#define _BVS_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/branchinstruction/branchinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// BVS - branch if overflow is set
//
// Operation:
// PC <- PC + (2 * offset) if V = 1
//  
// Condition Codes: Unaffected
//
// Tests the state of V bit (overflow) and causes a branch if the V bit is
// set. BVS is used to detect arithmetic overflow in the previous operation.
//
class CommonInstruction::BVS : public BranchInstruction, public WithFactory<BVS>
{
public:
    BVS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::BVS::BVS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BVS::execute ()
{
    executeBranchIf (isSet (PSW_V));
    return CpuData::Trap::None;
}

#endif // _BVS_H_