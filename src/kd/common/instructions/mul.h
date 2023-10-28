#ifndef _MUL_H_
#define _MUL_H_

#include "commoninstruction.h"
#include "kd/common/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// MUL - multiply
//
// Operation:
//  R, R+1 <- R x (src)
//
//  Condition Codes:
//  N: set if product is <0: cleared otherwise
//  Z: set if product is O: cleared otherwise
//  V: cleared
//  C: set if the result is less than-215 or greater than or equal to 2^15 - 1
//
// The contents of the destination register and source taken as two's complement
// integers are multiplied and stored in the destination register and the
// succeeding register (if R is even). If R is odd only the low order product
// is stored.
//
class CommonInstruction::MUL : public EisInstruction, public WithFactory<MUL>
{
public:
    MUL (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MUL::MUL (CpuData* cpu, u16 instruction)
    :
    EisInstruction (cpu, instruction)
{}

inline bool CommonInstruction::MUL::execute ()
{
    u16 regNr = getRegisterNr ();

    GeneralRegisters& registers = cpu_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!readOperand (&source))
        return false;

    s32 tmps32 = (s32)(s16)destination * (s16)source;
    registers[regNr] = (u16)(tmps32 >> 16);
    registers[regNr | 1] = (u16)tmps32;

    setPSW (ConditionCodes {.N = tmps32 < 0,
        .Z = tmps32 == 0,
        .V = false,
        .C = (tmps32 >= 0x7FFF) || (tmps32 < -0x8000)});

    return true;
}

#endif // _MUL_H_