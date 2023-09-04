#ifndef _DIV_H_
#define _DIV_H_

#include "kd11_nainstruction.h"
#include "../eisinstruction/eisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NAInstruction::DIV : public EisInstruction, public WithFactory<DIV>
{
public:
    DIV (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::DIV::DIV (CpuData* cpu, u16 instruction)
    :
    EisInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::DIV::execute ()
{
    u16 regNr = getRegisterNr ();

    CpuData::GeneralRegisters& registers = cpu_->registers ();
    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    if (source == 0)
    {
        setConditionCode (PSW_C);
        setConditionCode (PSW_V);
    }
    else
    {
        s32 quotient = tmps32 / (s16)source;
        s32 remainder = tmps32 % (s16)source;
        clearConditionCode (PSW_C);

        if ((s16)quotient != quotient)
        {
            setConditionCode (PSW_V);
        }
        else
        {
            registers[regNr] = (u16)quotient;
            registers[regNr | 1] = (u16)remainder;
            setConditionCodeIf_ClearElse (PSW_Z, !quotient);
            setConditionCodeIf_ClearElse (PSW_N, quotient < 0);
        }
    }

    return CpuData::Trap::None;
}

#endif // _DIV_H_