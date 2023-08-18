#ifndef _BICB_H_
#define _BICB_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BICB - bit clear byte
//
// Operation:
//  refer to BIC
// 
// Condition Codes:
//  refer to BIC
//
namespace KD11_NA
{
    class BICB : public DoubleOperandInstruction
    {
    public:
        BICB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BICB::BICB (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap BICB::execute ()
    {
        CondData<u8> source, destination;

        if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
            return CpuData::Trap::BusError;

        u8 tmp = (u8) (~source & destination);

        if (!writeDestinationOperand (tmp))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);
        clearConditionCode (PSW_V);

        return CpuData::Trap::None;
    }
}

#endif // _BICB_H_