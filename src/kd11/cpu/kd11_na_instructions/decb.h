#ifndef _DECB_H_
#define _DECB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
namespace KD11_NA
{
    class DECB : public SingleOperandInstruction
    {
    public:
        DECB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    DECB::DECB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap DECB::execute ()
    {
        CondData<u8> src = location_.byteContents ();
        if (!src.hasValue ())
            return CpuData::Trap::BusError;

        u8 tmp = (u8) (src - 1);

        location_.writeByte (tmp);

        setConditionCodeIf_ClearElse (PSW_V, src == 0000200);
        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);

        return CpuData::Trap::None;
    }
}

#endif // _DECB_H_