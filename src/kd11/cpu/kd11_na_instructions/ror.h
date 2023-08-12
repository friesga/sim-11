#ifndef _ROR_H_
#define _ROR_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// ROR - rotate right
//
// Operation:
//  (dst) <- (dst) rotated right one place
//
// Condition Codes:
//  N: set if the high-order bit of the result is set (result < 0);
//     cleared otherwise
//  Z: set if all bits of result - 0; cleared otherwise
//  V: loaded with the Exclusive OR of the N-bit and C-bi.t (as set by the
//     completion of the rotate operation)
//  C: loaded with the low-order bit of the destination
//
// Rotates all bits of the destination right one place. Bit 0 is loaded into
// the C-bit and the previous contents of the C-bit are loaded into bit 15 of
// the destination.
//
namespace KD11_NA
{
    class ROR : public SingleOperandInstruction
    {
    public:
        ROR (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    ROR::ROR (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap ROR::execute ()
    {
        CondData<u16> contents = location_.wordContents ();
        if (!contents.hasValue ())
            return CpuData::Trap::BusError;

        u16 cBit = isSet (PSW_C);
        u16 result = contents >> 1;
        if (cBit)
            result |= 0100000;

        location_.write (result);

        setConditionCodeIf_ClearElse (PSW_C, contents & 0000001);
        setConditionCodeIf_ClearElse (PSW_N, result & 0100000);
        setConditionCodeIf_ClearElse (PSW_Z, !result);
        setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

        return CpuData::Trap::None;
    }
}

#endif // _ROR_H_