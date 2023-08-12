#ifndef _SXT_H_
#define _SXT_H_

#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// SXT - sign extend
//
// Operation:
//  (dst) <- 0 if N bit is clear
//  (dst) <- 1 if N bit is set
//
// Condition Codes:
//  N: unaffected
//  Z: set if N bit clear
//  V: cleared
//  C: unaffected
//
// If the condition code bit N is set then a -1 is placed in the destination
// operand: if N bit is clear, then a 0 is placed in the destination operand.
//
namespace KD11_NA
{
    class SXT : public SingleOperandInstruction
    {
    public:
        SXT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        OperandLocation location_;
    };

    SXT::SXT (CpuData* cpu, u16 instruction)
        :
        SingleOperandInstruction (cpu, instruction),
        location_ {getOperandLocation (cpu_->registers ())}
    {}

    CpuData::Trap SXT::execute ()
    {
        u16 result = isSet (PSW_N) ? 0177777 : 0;

        location_.write (result);

        setConditionCodeIf_ClearElse (PSW_Z, !isSet (PSW_N));
        clearConditionCode (PSW_V);

        return CpuData::Trap::None;
    }
}

#endif // _SXT_H_