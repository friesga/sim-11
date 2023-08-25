#ifndef _COM_H_
#define _COM_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// COM - complement destination
//
// Operation:
//  (dst) <- ~(dst)
//
// Condition Codes:
//  N: set if most significant bit of result is set; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: set
//
// Replaces the contents of the destination address by their logical
// complement (each bit equal to 0 is set and each bit equal to 1 is cleared).
//
class KD11_NA::COM : public SingleOperandInstruction, public WithFactory<COM>
{
public:
    COM (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::COM::COM (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::COM::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return CpuData::Trap::BusError;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    if (!writeOperand (operand))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_V);
    setConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !operand);

    return CpuData::Trap::None;
}

#endif // _COM_H_