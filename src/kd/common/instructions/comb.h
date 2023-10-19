#ifndef _COMB_H_
#define _COMB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// COMB - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
class CommonInstruction::COMB : public SingleOperandInstruction, public WithFactory<COMB>
{
public:
    COMB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::COMB::COMB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::COMB::execute ()
{
    CondData<u8> operand;
    if (!readOperand (&operand))
        return CpuData::Trap::BusError;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    if (!writeOperand (operand.value ()))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_V);
    setConditionCode (PSW_C);
    setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !((u8)operand));

    return CpuData::Trap::None;
}

#endif // _COMB_H_