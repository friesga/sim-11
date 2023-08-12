#ifndef _BNE_H_
#define _BNE_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class BNE : public BranchInstruction
    {
    public:
        BNE (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BNE::BNE (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BNE::execute ()
    {
        executeBranchIf (!isSet (PSW_Z));
        return CpuData::Trap::None;
    }
}

#endif // _BNE_H_