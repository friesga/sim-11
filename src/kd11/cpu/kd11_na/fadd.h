#ifndef _FADD_H_
#define _FADD_H_

#include "kd11_na.h"
#include "../fisinstruction/fisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::FADD : public FISInstruction, public WithFactory<FADD>
{
public:
    FADD (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::FADD::FADD (CpuData* cpu, u16 instruction)
    :
    FISInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::FADD::execute ()
{
    return executeFISinstruction (getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 + f2; });
}

#endif // _FADD_H_