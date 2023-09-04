#ifndef _BGE_H_
#define _BGE_H_

#include "kd11_nainstruction.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BGE - branch if greater than or equal (to zero)
// 
// Operation:
//  PC <- PC + (2 * offset) if N xor V = 0
//
// Causes a branch if N and V are either both clear or both set. BGE is the
// complementary operation to BLT. Thus BGE will always cause a branch when it
// follows an operation that caused addition of two positive numbers. BGE will
// also cause a branch on a zero result.
//
class KD11_NAInstruction::BGE : public BranchInstruction, public WithFactory<BGE>
{
public:
    BGE (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BGE::BGE (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BGE::execute ()
{
    executeBranchIf ((isSet (PSW_N) ^ isSet (PSW_V)) == 0);
    return CpuData::Trap::None;
}

#endif // _BGE_H_