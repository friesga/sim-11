#ifndef _ADCB_H_
#define _ADCB_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
class CommonInstruction::ADCB : public SingleOperandInstruction, public WithFactory<ADCB>
{
public:
    ADCB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::ADCB::ADCB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::ADCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    u16 tmp = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8)(source + tmp);

    if (!writeOperand (destination))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_V, source == 0177 && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_C, source == 0377 && isSet (PSW_C));
    setConditionCodeIf_ClearElse (PSW_N, destination & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !destination);

    return CpuData::Trap::None;
}

#endif // _ADCB_H_