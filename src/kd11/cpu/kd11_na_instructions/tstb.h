#ifndef _TSTB_H_
#define _TSTB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// TSTB - test destination byte
//
// Operation:
//  refer to TST
// 
// Condition Codes:
//  refer to TST
//
namespace KD11_NA
{
    class TSTB : public SingleOperandInstruction
    {
    public:
        TSTB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    TSTB::TSTB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap TSTB::execute ()
    {
        CondData<u8> source;
        if (!readOperand (&source))
            return CpuData::Trap::BusError;

        clearConditionCode (PSW_V);
        clearConditionCode (PSW_C);
        setConditionCodeIf_ClearElse (PSW_N, source & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !source);

        return CpuData::Trap::None;
    }
}

#endif // _TSTB_H_