#ifndef _BVS_H_
#define _BVS_H_

#include "kd11_nainstruction.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NAInstruction::BVS : public BranchInstruction, public WithFactory<BVS>
{
public:
    BVS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BVS::BVS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BVS::execute ()
{
    executeBranchIf (isSet (PSW_V));
    return CpuData::Trap::None;
}

#endif // _BVS_H_