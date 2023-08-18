#ifndef _BITB_H_
#define _BITB_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
namespace KD11_NA
{
    class BITB : public DoubleOperandInstruction
    {
    public:
        BITB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BITB::BITB (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap BITB::execute ()
    {
        CondData<u8> source, destination;

        if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
            return CpuData::Trap::BusError;

        u16 tmp = source & destination;
        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);
        clearConditionCode (PSW_V);

        return CpuData::Trap::None;
    }
}

#endif // _BITB_H_