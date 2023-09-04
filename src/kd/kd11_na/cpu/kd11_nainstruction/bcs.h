#ifndef _BCS_H_
#define _BCS_H_

#include "kd11_nainstruction.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NAInstruction::BCS : public BranchInstruction, public WithFactory<BCS>
{
public:
    BCS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BCS::BCS (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BCS::execute ()
{
    executeBranchIf (isSet (PSW_C));
    return CpuData::Trap::None;
}

#endif // _BCS_H_