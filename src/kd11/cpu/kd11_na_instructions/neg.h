#ifndef _NEG_H_
#define _NEG_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// NEG - negate destination
//
// Operation:
//  (dst) <- -(dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is O: cleared otherwise
//  V: set if the result is 100000; cleared otherwise
//  C: cleared if the result is O; set otherwise
//
// Replaces the contents of the destination address by its two's complement.
// Note that 100000 is replaced by itself (in two's complement notation the
// most negative number has no positive counterpart).
//
namespace KD11_NA
{
    class NEG : public SingleOperandInstruction
    {
    public:
        NEG (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    NEG::NEG (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap NEG::execute ()
    {
        CondData<u16> operand = location_.wordContents ();
        if (!operand.hasValue ())
            return CpuData::Trap::BusError;

        // Negate the operand and write it to the operand location
        if (operand != 0100000)
            operand = -operand;
    
        location_.write (operand);

        setConditionCodeIf_ClearElse (PSW_V, operand == 0100000);
        setConditionCodeIf_ClearElse (PSW_N, operand & 0100000);
        setConditionCodeIf_ClearElse (PSW_Z, !operand);
        setConditionCodeIf_ClearElse (PSW_C, operand);

        return CpuData::Trap::None;
    }
}

#endif // _NEG_H_