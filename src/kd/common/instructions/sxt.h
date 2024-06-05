#ifndef _SXT_H_
#define _SXT_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class CommonInstruction::SXT : public SingleOperandInstruction
{
public:
    SXT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::SXT::SXT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::SXT::execute ()
{
    u16 result = isSet (PSW_N) ? 0177777 : 0;

    if (!writeOperand (result))
        return false;

    setPSW (ConditionCodes {.Z = !isSet (PSW_N),
        .V = false});

    return true;
}

#endif // _SXT_H_