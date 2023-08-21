#ifndef _CLR_H_
#define _CLR_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
namespace KD11_NA
{
    class CLR : public SingleOperandInstruction
    {
    public:
        CLR (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    CLR::CLR (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap CLR::execute ()
    {
        if (!writeOperand ((u16) 0))
            return CpuData::Trap::BusError;

        clearConditionCode (PSW_N | PSW_V | PSW_C);
        setConditionCode (PSW_Z);

        return CpuData::Trap::None;
    }
}

#endif // _CLR_H_