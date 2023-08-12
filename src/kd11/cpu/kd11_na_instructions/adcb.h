#ifndef _ADCB_H_
#define _ADCB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
namespace KD11_NA
{
    class ADCB : public SingleOperandInstruction
    {
    public:
        ADCB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    ADCB::ADCB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap ADCB::execute ()
    {
        CondData<u8> source = location_.byteContents ();
        if (!source.hasValue ())
            return CpuData::Trap::BusError;

        u16 tmp = isSet (PSW_C) ? 1 : 0;
        u16 destination = (u8) (source + tmp);

        if (!location_.writeByte (destination))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_V, source == 0177 && isSet (PSW_C));
        setConditionCodeIf_ClearElse (PSW_C, source == 0377 && isSet (PSW_C));
        setConditionCodeIf_ClearElse (PSW_N, destination & 0x80);
        setConditionCodeIf_ClearElse (PSW_Z, !destination);

        return CpuData::Trap::None;
    }
}

#endif // _ADCB_H_