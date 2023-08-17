#ifndef _CMP_H_
#define _CMP_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// CMP - compare source to destination
//
// Operation:
//  (src)=(dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow; that is. operands were of
//     opposite signs and the sign of the destination was the same as the
//     sign of the result: cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
// Compares the source and destination operands and sets the condition codes,
// which may then be used for arithmetic and logical conditional branches.
// Both operands are unaffected. The only action is to set the condition codes.
//
namespace KD11_NA
{
    class CMP : public DoubleOperandInstruction
    {
    public:
        CMP (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    CMP::CMP (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap CMP::execute ()
    {
        CondData<u16> source, destination;

        if (!readSourceOperand (&source) || 
                !readDestinationWordOperand (&destination))
            return CpuData::Trap::BusError;

        u16 tmp = source - destination;
        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);
        setConditionCodeIf_ClearElse (PSW_V, ((source & 0x8000) != (destination & 0x8000)) \
            && ((destination & 0x8000) == (tmp & 0x8000)));
        setConditionCodeIf_ClearElse (PSW_C, ((u32) source - (u32) destination) & 0x10000);
        
        return CpuData::Trap::None;
    }
}

#endif // _CMP_H_