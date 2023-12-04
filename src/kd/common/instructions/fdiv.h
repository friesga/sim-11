#ifndef _FDIV_H_
#define _FDIV_H_

#include "commoninstruction.h"
#include "kd/common/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// FDIV - floating divide
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] / [(R),(R)+2 ]
//  if result >= 2^-128; else [(R)+4, (R)+6)] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: cleared
//  C: cleared
//
// Divides the A Argument by the B Argument and stores the result in the
// A Argument position on the stack. If the divisor (B Argument) is equal to
// zero, the stack is left untouched.
//
// A <- A / B
//
class CommonInstruction::FDIV : public FISInstruction, public WithFactory<FDIV>
{
public:
    FDIV (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::FDIV::FDIV (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    FISInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::FDIV::execute ()
{
    return executeFISinstruction (getRegister (),
        [](Float f1, Float f2) { return f2.value () != 0; },
        [](Float f1, Float f2) { return f1 / f2; });
}

#endif // _FDIV_H_