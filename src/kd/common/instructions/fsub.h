#ifndef _FSUB_H_
#define _FSUB_H_

#include "commoninstruction.h"
#include "kd/common/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// FSUB - floating subtract
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] - [(R), (R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: cleared
//
// Sutracts the B Argument from the A Argument and stores the result in
// the A Argument position on the stack.
//
// A <- A - B
//
class CommonInstruction::FSUB : public FISInstruction, public WithFactory<FSUB>
{
public:
    FSUB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::FSUB::FSUB (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    FISInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::FSUB::execute ()
{
    return executeFISinstruction (getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 - f2; });
}

#endif // _FSUB_H_