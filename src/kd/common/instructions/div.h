#ifndef _DIV_H_
#define _DIV_H_

#include "commoninstruction.h"
#include "kd/common/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// DIV - divide
//
// Operation:
//  R,R+1 <- R,R+1 / (src)
//
// Condition Codes:
//  N: set if quotient <0: cleared otherwise
//  Z: set if quotient = 0: cleared otherwise
//  V: set if source = 0 or if the absolute value of the register is larger
//     than the absolute value of the source. (In this case the instruction
//     is aborted because the quotient would exceed 15 bits.)
//  C: set if divide 0 attempted; cleared otherwise
//
// The 32-bit two's complement integer in R and R+1 is divided by the source
// operand. The quotient is left in R: the remainder in R+1. Division will be
// performed so that the remainder is of the same sign as the dividend. R must
// be even.
//
class CommonInstruction::DIV : public EisInstruction, public WithFactory<DIV>
{
public:
    DIV (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::DIV::DIV (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::DIV::execute ()
{
    u16 regNr = getRegisterNr ();

    GeneralRegisters& registers = cpuData_->registers ();
    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!readOperand (&source))
        return false;

    if (source == 0)
    {
        setPSW (ConditionCodes {.V = true, .C = true});
    }
    else
    {
        s32 quotient = tmps32 / (s16)source;
        s32 remainder = tmps32 % (s16)source;

        if ((s16)quotient != quotient)
        {
            setPSW (ConditionCodes {.V = true, .C = false});
        }
        else
        {
            registers[regNr] = (u16)quotient;
            registers[regNr | 1] = (u16)remainder;
            setPSW (ConditionCodes {.N = quotient < 0,
                .Z = quotient == 0,
                .C = false});
        }
    }

    return true;
}

#endif // _DIV_H_