#ifndef _BEQ_H_
#define _BEQ_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::BEQ : public BranchInstruction, public WithFactory<BEQ>
{
public:
    BEQ (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::BEQ::BEQ (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::BEQ::execute ()
{
    executeBranchIf (isSet (PSW_Z));
    return CpuData::Trap::None;
}

#endif // _BEQ_H_