#ifndef _BEQ_H_
#define _BEQ_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BEQ - branch if queal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z = 1
// 
// Tests the state of the Z·bit and causes a branch if Z is set. As an
// example, it is used to test equality following a CMP operation, to test
// that no bits set in the destination were also set in the source following
// a BIT operation, and generally, to test that the result of the previous
// operation was zero.
//
class CommonInstruction::BEQ : public BranchInstruction, public WithFactory<BEQ>
{
public:
    BEQ (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BEQ::BEQ (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BEQ::execute ()
{
    executeBranchIf (isSet (PSW_Z));
    return true;
}

#endif // _BEQ_H_