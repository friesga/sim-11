#ifndef _DEC_H_
#define _DEC_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// DEC - decrement destination
//
// Operation:
//  (dst) <- (dst) - 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: not affected
//
// Subtract 1 from the contents of the destination
//
class CommonInstruction::DEC : public SingleOperandInstruction, public WithFactory<DEC>
{
public:
    DEC (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::DEC::DEC (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::DEC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;
    if (!writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = contents == 0100000});

    return true;
}

#endif // _DEC_H_