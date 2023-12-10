#ifndef _BLT_H_
#define _BLT_H_

#include "commoninstruction.h"
#include "kd/common/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BLT - branch if less than (zero)
//
// Operation:
//  PC <- PC + (2 * offset) if N xor V = 1
//
// Causes a branch if the "Exclusive Or" of the N and V bits are 1. Thus BLT
// will always branch following an operation that added two negative numbers,
// even if overflow occurred. In particular, BLT will always cause a branch if
// it follows a CMP instruction operating on a negative source and a positive
// destination (even if overflow occurred). Further. BLT will never cause a
// branch when it follows a CMP instruction operating on a positive source and
// negative destination. BLT will not cause a branch if the result of the
// previous operation was zero (without overflow).
//
class CommonInstruction::BLT : public BranchInstruction, public WithFactory<BLT>
{
public:
    BLT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BLT::BLT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BLT::execute ()
{
    executeBranchIf (isSet (PSW_N) ^ isSet (PSW_V));
    return true;
}

#endif // _BLT_H_