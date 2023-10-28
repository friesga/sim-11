#ifndef _BVS_H_
#define _BVS_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
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
    bool execute () override;
};

inline CommonInstruction::BVS::BVS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

inline bool CommonInstruction::BVS::execute ()
{
    executeBranchIf (isSet (PSW_V));
    return true;
}

#endif // _BVS_H_