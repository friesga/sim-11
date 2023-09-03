#ifndef _BLE_H_
#define _BLE_H_

#include "kd11_na.h"
#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// BLE - branch if less than or equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 1
//
// Operation is similar to BLT but in addition will cause a branch if the
// result of the previous operation was zero.
//
class KD11_NAInstruction::BLE : public BranchInstruction, public WithFactory<BLE>
{
public:
    BLE (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BLE::BLE (CpuData* cpu, u16 instruction)
    :
    BranchInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BLE::execute ()
{
    executeBranchIf (isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V)));
    return CpuData::Trap::None;
}

#endif // _BLE_H_