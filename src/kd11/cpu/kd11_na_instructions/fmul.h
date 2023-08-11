#ifndef _FMUL_H_
#define _FMUL_H_

#include "../fisinstruction/fisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class FMUL : public FISInstruction
    {
    public:
        FMUL (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    FMUL::FMUL (CpuData* cpu, u16 instruction)
        :
        FISInstruction (cpu, instruction)
    {}

    CpuData::Trap FMUL::execute ()
    {
        return executeFISinstruction (getRegister (),
            [] (Float f1, Float f2) {return true;},
            [] (Float f1, Float f2) {return f1 * f2;});
    }
}

#endif // _FMUL_H_