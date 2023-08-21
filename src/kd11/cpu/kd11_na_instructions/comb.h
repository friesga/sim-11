#ifndef _COMB_H_
#define _COMB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// COMB - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
namespace KD11_NA
{
    class COMB : public SingleOperandInstruction
    {
    public:
        COMB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    COMB::COMB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap COMB::execute ()
    {
        CondData<u8> operand;
        if (!readOperand (&operand))
            return CpuData::Trap::BusError;

        // Complement the operand and write it to the operand location
        operand = ~operand;
        if (!writeOperand (operand))
            return CpuData::Trap::BusError;

        clearConditionCode (PSW_V);
        setConditionCode (PSW_C);
        setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !((u8) operand));

        return CpuData::Trap::None;
    }
}

#endif // _COMB_H_