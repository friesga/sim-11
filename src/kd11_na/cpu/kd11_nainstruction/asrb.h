#ifndef _ASRB_H_
#define _ASRB_H_

#include "kd11_nainstruction.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// ASRB - arithmetic shift right byte
//
// Operation:
//  refer to ASR
// 
// Condition Codes:
//  refer to ASR
//
// Same as ASR instruction with the distinction that for odd adresses bit 15
// is reproduced and for even addresses bit 7 is reproduced.
//
class KD11_NAInstruction::ASRB : public SingleOperandInstruction, public WithFactory<ASRB>
{
public:
    ASRB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::ASRB::ASRB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::ASRB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    u8 result = source;
    if (result & 0x80)
    {
        result >>= 1;
        result |= 0x80;
    }
    else
        result >>= 1;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_C, source & 1);
    setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

    return CpuData::Trap::None;
}

#endif // _ASRB_H_