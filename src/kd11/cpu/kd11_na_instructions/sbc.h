#ifndef _SBC_H_
#define _SBC_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// SBC - subtract carry
//
// Operation
//  (dst) <- (dst) - (C)
//
// Condition Codes:
//  N: set if result <O; cleared otherwise
//  Z: set if result O; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: set if (dst) was 0 and C was 1; cleared otherwise
//
// Subtracts the contents of the C-bit from the destination. This permits the
// carry from the subtraction of two low-order words to be subtracted from the
// high order part of the result.
namespace KD11_NA
{
    class SBC : public SingleOperandInstruction
    {
    public:
        SBC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    SBC::SBC (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap SBC::execute ()
    {
        CondData<u16> contents = location_.wordContents ();
        if (!contents.hasValue ())
            return CpuData::Trap::BusError;

        u16 cBit = isSet (PSW_C) ? 1 : 0;
        u16 result = contents - cBit;

        location_.write (result);

        setConditionCodeIf_ClearElse (PSW_V, contents == 0100000);
        setConditionCodeIf_ClearElse (PSW_C, !contents && isSet (PSW_C));
        setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
        setConditionCodeIf_ClearElse (PSW_Z, !result);

        return CpuData::Trap::None;
    }
}

#endif // _SBC_H_