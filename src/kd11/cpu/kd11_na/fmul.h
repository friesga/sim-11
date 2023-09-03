#ifndef _FMUL_H_
#define _FMUL_H_

#include "kd11_na.h"
#include "../fisinstruction/fisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// FMUL - floating multiply
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6] * [(R), (R)+2]
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: cleared
//  C: cleared
//
// Multiplies the A Argument by the B Argument and stores the result in the
// A Argument position on the stack.
//
// A <- A * B
//
class KD11_NAInstruction::FMUL : public FISInstruction, public WithFactory<FMUL>
{
public:
    FMUL (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::FMUL::FMUL (CpuData* cpu, u16 instruction)
    :
    FISInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::FMUL::execute ()
{
    return executeFISinstruction (getRegister (),
        [](Float f1, Float f2) { return true; },
        [](Float f1, Float f2) { return f1 * f2; });
}

#endif // _FMUL_H_