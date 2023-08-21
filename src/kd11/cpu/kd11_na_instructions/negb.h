#ifndef _NEGB_H_
#define _NEGB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
namespace KD11_NA
{
    class NEGB : public SingleOperandInstruction
    {
    public:
        NEGB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    NEGB::NEGB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap NEGB::execute ()
    {
        CondData<u8> operand;
        if (!readOperand (&operand))
            return CpuData::Trap::BusError;

        if (operand != 0200)
            operand = -operand;

        if (!writeOperand (operand))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_V, operand == 0200);
        setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !operand);
        setConditionCodeIf_ClearElse (PSW_C, operand);

        return CpuData::Trap::None;
    }
}

#endif // _NEGB_H_