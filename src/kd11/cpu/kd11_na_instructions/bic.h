#ifndef _BIC_H_
#define _BIC_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// BIC - bit clear
//
// Operation:
//  (dst) <- (src) ^ (dst)
//
// Condition Codes:
//  N: set if high order bit of result set; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: not affected
//
// Clears each bit in the destination that corresponds to a set bit in the
// source. The original contents of the destination are lost. The contents of
// the source are unaffected.
//
namespace KD11_NA
{
    class BIC : public DoubleOperandInstruction
    {
    public:
        BIC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BIC::BIC (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap BIC::execute ()
    {
        CondData<u16> source, destination;

        if (!readSourceWordOperand (&source) || 
                !readDestinationWordOperand (&destination))
            return CpuData::Trap::BusError;

        u16 result = ~source & destination;

        if (!writeDestinationWordOperand (result))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
        setConditionCodeIf_ClearElse (PSW_Z, !result);
        clearConditionCode (PSW_V);
        
        return CpuData::Trap::None;
    }
}

#endif // _BIC_H_