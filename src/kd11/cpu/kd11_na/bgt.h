#ifndef _BGT_H_
#define _BGT_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BGT - branch if greater than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 0
//
// Operation of BGT is similar to BGE, except BGT will not cause a branch on
// a zero result.
class KD11_NA::BGT : public BranchInstruction, public WithFactory<BGT>
{
public:
    BGT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::BGT::BGT (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::BGT::execute ()
{
    executeBranchIf ((isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V))) == 0);
    return CpuData::Trap::None;
}

#endif // _BGT_H_