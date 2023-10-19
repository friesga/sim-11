#ifndef _BCS_H_
#define _BCS_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BCS - branch if carry is set
//
// Operation:
//  PC <- PC + (2 * offset) if C = 1
//  
// Condition Codes: Unaffected
// 
// Tests the state of the C-bit and causes a branch if C is set. It is used
// to test for a carry in the result of a previous operation.
//
class CommonInstruction::BCS : public BranchInstruction, public WithFactory<BCS>
{
public:
    BCS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::BCS::BCS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::BCS::execute ()
{
    executeBranchIf (isSet (PSW_C));
    return CpuData::Trap::None;
}

#endif // _BCS_H_