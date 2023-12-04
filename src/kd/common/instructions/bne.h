#ifndef _BNE_H_
#define _BNE_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BNE - branch if not equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z = 0
//
// Tests the state of the Z-bit and causes a branch if the Z-bit is clear.
// BNE is the complementary operation to BEQ. It is used to test inequality
// following a CMP, to test that some bits set in the destination were also
// in the source, following a BIT, and generally, to test that the result of
// the previous operation was not zero.
//
class CommonInstruction::BNE : public BranchInstruction, public WithFactory<BNE>
{
public:
    BNE (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BNE::BNE (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BNE::execute ()
{
    executeBranchIf (!isSet (PSW_Z));
    return true;
}

#endif // _BNE_H_