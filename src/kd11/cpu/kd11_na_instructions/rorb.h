#ifndef _RORB_H_
#define _RORB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// RORB - rotate right byte
//
// Operation:
//  refer to ROR
// 
// Condition Codes:
//  refer to ROR
//
// Same as ROR instruction with the distintion that for odd adresses the
// carry bit is loaded in bit 15 of the word and for even addresses the carry
// bit is loaded in bit 7 of the word.
//
namespace KD11_NA
{
    class RORB : public SingleOperandInstruction
    {
    public:
        RORB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    RORB::RORB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap RORB::execute ()
    {
        CondData<u8> source = location_.byteContents ();
        if (!source.hasValue ())
            return CpuData::Trap::BusError;

        u16 result = source >> 1;
        if (isSet (PSW_C))
            result |= 0x80;

        if (!location_.writeByte (result))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_C, source & 0x01);
        setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !result);
        setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

        return CpuData::Trap::None;
    }
}

#endif // _RORB_H_