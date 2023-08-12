#ifndef _INCB_H_
#define _INCB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
namespace KD11_NA
{
    class INCB : public SingleOperandInstruction
    {
    public:
        INCB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    INCB::INCB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap INCB::execute ()
    {
        CondData<u8> src = location_.byteContents ();
        if (!src.hasValue ())
            return CpuData::Trap::BusError;

        u8 tmp = (u8) (src + 1);

        location_.writeByte (tmp);

        setConditionCodeIf_ClearElse (PSW_V, src == 000177);
        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);

        return CpuData::Trap::None;
    }
}

#endif // _INCB_H_