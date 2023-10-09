#ifndef _SWAB_H_
#define _SWAB_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// SWAB - swap bytes
//
// Operation:
//  byte 1/byte 0 <- byte 0/byte 1
//
// Condition Codes:
//  N: set if high-order bit of low-order byte (bit 7) of result is set;
//     cleared otherwise
//  Z: set if low-order byte of result = O; cleared otherwise
//  V: cleared
//  C: cleared
// 
// Exchanges high-order byte and low-order byte of the destination
// word (destination must be a word address).
//
class CommonInstruction::SWAB : public SingleOperandInstruction, public WithFactory<SWAB>
{
public:
    SWAB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::SWAB::SWAB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::SWAB::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return CpuData::Trap::BusError;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    if (!writeOperand (operand.value ()))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !((u8)operand));
    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);

    return CpuData::Trap::None;
}


#endif // _SWAB_H_