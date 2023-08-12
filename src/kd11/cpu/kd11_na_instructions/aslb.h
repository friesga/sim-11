#ifndef _ASLB_H_
#define _ASLB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// ASLB - arithmetic shift left byte
//
// Operation:
//  refer to ASL
// 
// Condition Codes:
//  refer to ASL
//
// Same as ASL instruction with the distinction that for odd adresses the
// carry bit is loaded from bit 15 of the word and bit 8 is loaded with a zero
// and for even addresses the carry bit is loaded from bit 7 and bit 0 is
// loaded with a zero.
//
namespace KD11_NA
{
    class ASLB : public SingleOperandInstruction
    {
    public:
        ASLB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    ASLB::ASLB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap ASLB::execute ()
    {
        CondData<u8> source = location_.byteContents ();
        if (!source.hasValue ())
            return CpuData::Trap::BusError;

        u16 result = (u8) (source << 1);

        if (!location_.writeByte (result))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_C, source & 0x80);
        setConditionCodeIf_ClearElse (PSW_N, result & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !result);
        setConditionCodeIf_ClearElse (PSW_V, isSet (PSW_N) ^ isSet (PSW_C));

        return CpuData::Trap::None;
    }
}

#endif // _ASLB_H_