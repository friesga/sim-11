#ifndef _INCB_H_
#define _INCB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
namespace KD11_NA
{
    class INCB : public SingleOperandInstruction
    {
    public:
        INCB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    INCB::INCB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap INCB::execute ()
    {
        CondData<u8> source;
        if (!readOperand (&source))
            return CpuData::Trap::BusError;

        u8 result = (u8) (source + 1);

        if (!writeOperand (result))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_V, source == 000177);
        setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !result);

        return CpuData::Trap::None;
    }
}

#endif // _INCB_H_