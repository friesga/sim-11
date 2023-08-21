#ifndef _SBCB_H_
#define _SBCB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// SBCB - subtract carry byte
//
// Operation:
//  refer to SBC
// 
// Condition Codes:
//  refer to SBC
//
namespace KD11_NA
{
    class SBCB : public SingleOperandInstruction
    {
    public:
        SBCB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    SBCB::SBCB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap SBCB::execute ()
    {
        CondData<u8> source;
        if (!readOperand (&source))
            return CpuData::Trap::BusError;

        u16 cBit = isSet (PSW_C) ? 1 : 0;
        u8 destination = (u8) (source - cBit);

        if (!writeOperand (destination))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_V, source == 0200);
        setConditionCodeIf_ClearElse (PSW_C, !source && isSet (PSW_C));
        setConditionCodeIf_ClearElse (PSW_N, destination & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !destination);

        return CpuData::Trap::None;
    }
}

#endif // _SBCB_H_