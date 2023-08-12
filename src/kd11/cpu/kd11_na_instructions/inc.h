#ifndef _INC_H_
#define _INC_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// INC - increment destination
//
// Operation:
//  (dst) <- (dst) + 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is O; cleared otherwise
//  V: set if (dst) held 077777; cleared otherwise
//  C: not affected
//
// Add one to contents of destination
//
namespace KD11_NA
{
    class INC : public SingleOperandInstruction
    {
    public:
        INC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    INC::INC (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap INC::execute ()
    {
        CondData<u16> contents = location_.wordContents ();
        if (!contents.hasValue ())
            return CpuData::Trap::BusError;

        // Increment the operand and write it to the operand location
        u16 result = contents + 1;
        location_.write (result);

        setConditionCodeIf_ClearElse (PSW_V, contents == 077777);
        setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
        setConditionCodeIf_ClearElse (PSW_Z, !result);

        return CpuData::Trap::None;
    }
}

#endif // _INC_H_