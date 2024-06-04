#ifndef _BLE_H_
#define _BLE_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/branchinstruction/branchinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BLE - branch if less than or equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 1
//
// Operation is similar to BLT but in addition will cause a branch if the
// result of the previous operation was zero.
//
class CommonInstruction::BLE : public BranchInstruction, public WithFactory<BLE>
{
public:
    BLE (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BLE::BLE (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    BranchInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BLE::execute ()
{
    executeBranchIf (isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V)));
    return true;
}

#endif // _BLE_H_