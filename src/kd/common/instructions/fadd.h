#ifndef _FADD_H_
#define _FADD_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// FADD - floating add
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6]+[(R),(R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: cleared
//
// Adds the A argument to the B argument and stores the result in the
// A Argument position on the stack. General register R is used as the stack
// pointer for the operation.
// 
// A <- A + B
//
class CommonInstruction::FADD : public FISInstruction, public WithFactory<FADD>
{
public:
    FADD (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::FADD::FADD (CpuData* cpu, u16 instruction)
    :
    FISInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::FADD::execute ()
{
    return executeFISinstruction (getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

#endif // _FADD_H_