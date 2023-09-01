#ifndef _SWAB_H_
#define _SWAB_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::SWAB : public SingleOperandInstruction, public WithFactory<SWAB>
{
public:
    SWAB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::SWAB::SWAB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::SWAB::execute ()
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