#ifndef _DECB_H_
#define _DECB_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
class CommonInstruction::DECB : public SingleOperandInstruction, public WithFactory<DECB>
{
public:
    DECB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::DECB::DECB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::DECB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    u8 result = (u8)(source - 1);

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_V, source == 0000200);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !result);

    return CpuData::Trap::None;
}

#endif // _DECB_H_