#ifndef _ASHC_H_
#define _ASHC_H_

#include "kd11_na.h"
#include "../eisinstruction/eisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// ASHC - arithemetic shift combined
//
// Operation:
//  R, R+1 <- R, R+1 The double word is shifted NN places to the right or
//  left, where NN =low order six bits of source
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if sign bit changes during the shift; cleared otherwise
//  C: loaded with high order bit when left shift; loaded with low order bit
//     when right shift (loaded with the last bit shifted out of the 32-bit
//     operand)
//
// The contents of the register and the register ORed with one are treated as
// one 32 bit word, R + 1 (bits 0-15) and R (bits 16-31) are shifted right or
// left the number of times specified by the shift count. The shift count is
// taken as the low order 6 bits of the source operand. This number ranges
// from -32 to +31. Negative is a right shift and positive is a left shift.
// 
// When the register chosen is an odd number the register and the register
// OR'ed with one are the same. In this case the right shift becomes a rotate
// (for up to a shift of 16). The 16 bit word is rotated right the number of
// bits specified by the shift count.
//
class KD11_NAInstruction::ASHC : public EisInstruction, public WithFactory<ASHC>
{
public:
    ASHC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::ASHC::ASHC (CpuData* cpu, u16 instruction)
    :
    EisInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::ASHC::execute ()
{
    u16 regNr = getRegisterNr ();
    CpuData::GeneralRegisters& registers = cpu_->registers ();
    u16 dst = registers[regNr];

    s32 tmps32 = (registers[regNr] << 16) | registers[regNr | 1];

    CondData<u16> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    if ((source & 0x3F) == 0x20)
    {
        // Negative; 32 right
        setConditionCodeIf_ClearElse (PSW_C, tmps32 & 0x80000000);
        clearConditionCode (PSW_V);
        if (isSet (PSW_C))
            tmps32 = 0xFFFFFFFF;
        else
            tmps32 = 0;
    }
    else if (source & 0x20)
    {
        // Negative - shift right
        s32 stmp2;
        source = (~source & 0x1F) + 1;
        stmp2 = tmps32 >> (source - 1);
        tmps32 >>= source;
        setConditionCodeIf_ClearElse (PSW_C, stmp2 & 1);
    }
    else if ((source & 0x1F) == 0)
    {
        // Zero - don't shift
        clearConditionCode (PSW_V);
        clearConditionCode (PSW_C);
    }
    else
    {
        // Positive - shift left
        s32 stmp2;
        source = source & 0x1F;
        stmp2 = tmps32 << (source - 1);
        tmps32 <<= source;
        setConditionCodeIf_ClearElse (PSW_C, stmp2 & 0x80000000);
        setConditionCodeIf_ClearElse (PSW_V, !!(dst & 0x8000)
            != !!(tmps32 & 0x80000000));
    }
    registers[regNr] = (u16)(tmps32 >> 16);
    registers[regNr | 1] = (u16)tmps32;
    setConditionCodeIf_ClearElse (PSW_N, tmps32 & 0x80000000);
    setConditionCodeIf_ClearElse (PSW_Z, !tmps32);

    return CpuData::Trap::None;
}

#endif // _ASHC_H_