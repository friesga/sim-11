#ifndef _CLRB_H_
#define _CLRB_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
namespace KD11_NA
{
    class CLRB : public SingleOperandInstruction
    {
    public:
        CLRB (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    CLRB::CLRB (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap CLRB::execute ()
    {
        if (!location_.writeByte (0))
            return CpuData::Trap::BusError;

        clearConditionCode (PSW_N | PSW_V | PSW_C);
        setConditionCode (PSW_Z);

        return CpuData::Trap::None;
    }
}

#endif // _CLRB_H_