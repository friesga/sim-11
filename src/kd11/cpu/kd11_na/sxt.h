#ifndef _SXT_H_
#define _SXT_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

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
class KD11_NA::SXT : public SingleOperandInstruction, public WithFactory<SXT>
{
public:
    SXT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::SXT::SXT (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::SXT::execute ()
{
    u16 result = isSet (PSW_N) ? 0177777 : 0;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_Z, !isSet (PSW_N));
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _SXT_H_