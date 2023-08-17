#ifndef _CMPB_H_
#define _CMPB_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// CMPB - compare source to destination byte
// 
// Operation:
//  refer to CMP
// 
// Condition Codes:
//  refer to CMP
//
namespace KD11_NA
{
    class CMPB : public DoubleOperandInstruction
    {
    public:
        CMPB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    CMPB::CMPB (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap CMPB::execute ()
    {
        CondData<u8> source, destination;

        if (!readSourceOperand (&source) || !readDestinationByteOperand (&destination))
            return CpuData::Trap::BusError;

        u16 tmp = (u8) (source - destination);

        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);
        setConditionCodeIf_ClearElse (PSW_V, ((source & 0x80) != (destination & 0x80)) \
            && ((destination & 0x80) == (tmp & 0x80)));
        setConditionCodeIf_ClearElse (PSW_C, (source - destination) & 0x100);

        return CpuData::Trap::None;
    }
}

#endif // _CMPB_H_