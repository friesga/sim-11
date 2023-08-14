#ifndef _BLE_H_
#define _BLE_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BLE - branch if less than or equal (to zero)
//
// Operation:
//  PC <- PC + (2 * offset) if Z or (N xor V) = 1
//
// Operation is similar to BLT but in addition will cause a branch if the
// result of the previous operation was zero.
//
namespace KD11_NA
{
    class BLE : public BranchInstruction
    {
    public:
        BLE (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BLE::BLE (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BLE::execute ()
    {
        executeBranchIf (isSet (PSW_Z) || (isSet (PSW_N) ^ isSet (PSW_V)));
        return CpuData::Trap::None;
    }
}

#endif // _BLE_H_