#ifndef _MOV_H_
#define _MOV_H_

#include "../doubleoperandinstruction/doubleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// MOV - move source to destination
//
// Operation:
//  (dst) <- (src)
//
// Condition Codes:
//  N: set if (src) < 0: cleared otherwise
//  Z: set if (src) = 0: cleared otherwise
//  V: cleared
//  C: not affected
//
// Moves the source operand to the destination location. The previous
// contents of the destination are lost. The contents of the source address
// are not affected.
//
namespace KD11_NA
{
    class MOV : public DoubleOperandInstruction
    {
    public:
        MOV (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    MOV::MOV (CpuData* cpu, u16 instruction)
        :
        DoubleOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap MOV::execute ()
    {
        if (!getSourceWordOperand ())
            return CpuData::Trap::BusError;

        OperandLocation destOperandLocation =
            getDestinationOperandLocation (cpu_->registers ());

        if (!destOperandLocation.write (source_))
            return CpuData::Trap::BusError;

        setConditionCodeIf_ClearElse (PSW_N, source_ & 0100000);
        setConditionCodeIf_ClearElse (PSW_Z, !source_);
        clearConditionCode (PSW_V);

        return CpuData::Trap::None;
    }
}

#endif // _MOV_H_