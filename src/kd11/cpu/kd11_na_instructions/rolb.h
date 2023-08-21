#ifndef _ROLB_H_
#define _ROLB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// ROLB - rotate left byte
//
// Operation:
//  refer to ROL
// 
// Condition Codes:
//  refer to ROL
//
// Same as ROL instruction with the distinction that for odd adresses the
// carry bit is loaded with bit 15 of the word and for even addresses the carry
// bit is loaded with bit 7 of the word.
//
namespace KD11_NA
{
    class ROLB : public SingleOperandInstruction
    {
    public:
        ROLB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    ROLB::ROLB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap ROLB::execute ()
    {
        CondData<u8> source;
        if (!readOperand (&source))
            return CpuData::Trap::BusError;

        u8 result = (u8) (source << 1);
        if (isSet (PSW_C))
            result |= 0x01;

        if (!writeOperand (result))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_C, source & 0x80);
        setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !result);
        setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

        return CpuData::Trap::None;
    }
}

#endif // _ROLB_H_