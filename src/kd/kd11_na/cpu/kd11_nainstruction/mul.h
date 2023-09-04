#ifndef _MUL_H_
#define _MUL_H_

#include "kd11_nainstruction.h"
#include "../eisinstruction/eisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NAInstruction::MUL : public EisInstruction, public WithFactory<MUL>
{
public:
    MUL (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::MUL::MUL (CpuData* cpu, u16 instruction)
    :
    EisInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::MUL::execute ()
{
    u16 regNr = getRegisterNr ();

    CpuData::GeneralRegisters& registers = cpu_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    s32 tmps32 = (s32)(s16)destination * (s16)source;
    registers[regNr] = (u16)(tmps32 >> 16);
    registers[regNr | 1] = (u16)tmps32;

    clearConditionCode (PSW_V);
    setConditionCodeIf_ClearElse (PSW_N, tmps32 < 0);
    setConditionCodeIf_ClearElse (PSW_Z, !tmps32);
    setConditionCodeIf_ClearElse (PSW_C, (tmps32 >= 0x7FFF) || (tmps32 < -0x8000));

    return CpuData::Trap::None;
}

#endif // _MUL_H_