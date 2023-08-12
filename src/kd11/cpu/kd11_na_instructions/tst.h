#ifndef _TST_H_
#define _TST_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// TST - test destination
//
// Operation
// (dst) <- (dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: cleared
//
namespace KD11_NA
{
    class TST : public SingleOperandInstruction
    {
    public:
        TST (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    TST::TST (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap TST::execute ()
    {
        CondData<u16> contents = location_.wordContents ();
        if (!contents.hasValue ())
            return CpuData::Trap::BusError;

        clearConditionCode (PSW_V);
        clearConditionCode (PSW_C);
        setConditionCodeIf_ClearElse (PSW_N, contents & 0100000);
        setConditionCodeIf_ClearElse (PSW_Z, !contents);

        return CpuData::Trap::None;
    }
}

#endif // _TST_H_