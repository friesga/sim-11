#ifndef _ADD_H_
#define _ADD_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// ADD - add source to destination
//
// Operation:
//  (dst) <- (src) + (dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation;
//     that is both operands were of the same sign and the result was of the
//     opposite sign; cleared otherwise
//  C: set if there was a carry from the most significant bit of the result;
//     cleared otherwise
//
// Adds the source operand to the destinatign operand and stores the result at
// the destination address. The original contents of the destination are lost.
// The contents of the source are not affected. Two's complement addition is
// performed.
//
namespace KD11_NA
{
    class ADD : public DoubleOperandInstruction
    {
    public:
        ADD (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    ADD::ADD (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap ADD::execute ()
    {
        CondData<u16> source, destination;

        if (!readSourceOperand (&source) || 
                !readDestinationOperand (&destination))
            return CpuData::Trap::BusError;

        u16 result = source + destination;

        if (!writeDestinationWordOperand (result))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
        setConditionCodeIf_ClearElse (PSW_Z, !result);
        setConditionCodeIf_ClearElse (PSW_V, ((source & 0x8000) == (destination & 0x8000)) \
            && ((destination & 0x8000) != (result & 0x8000)));
        setConditionCodeIf_ClearElse (PSW_C, ((u32)source + (u32)destination) & 0x10000);

        return CpuData::Trap::None;
    }
}

#endif // _ADD_H_