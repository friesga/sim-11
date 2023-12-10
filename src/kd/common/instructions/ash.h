#ifndef _ASH_H_
#define _ASH_H_

#include "commoninstruction.h"
#include "kd/common/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// ASH - shift arithmetically
//
// Operation:
//  R <- R shifted arithmetically NN places to right or left where
//  NN = low order 6 bits of source
//
// Condition Codes:
//  N: set if result <O; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if sign of register changed during shift; cleared otherwise
//  C: loaded from last bit shifted out of register
//
// The contents of the register are shifted right or left the number of times
// specified by the shift count. The shift count is taken as the low order
// 6 bits of the source operand. This number ranges from -32 to + 31. Negative
// is a a right shift and positive is a left shift.
//
class CommonInstruction::ASH : public EisInstruction, public WithFactory<ASH>
{
public:
    ASH (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ASH::ASH (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::ASH::execute ()
{
    u16 regNr = getRegisterNr ();
    u16 tmp {0};
    GeneralRegisters& registers = cpuData_->registers ();
    u16 destination = registers[regNr];

    CondData<u16> source;
    if (!readOperand (&source))
        return false;

    if (source & 0x20)
    {
        // Negative - shift right
        s16 stmp, stmp2;
        source = (~source & 0x1F) + 1;
        stmp = (s16)destination;
        stmp2 = stmp >> (source - 1);
        stmp >>= source;
        tmp = (u16)stmp;
        setPSW (ConditionCodes {.V = false, .C = (bool) (stmp2 & 1)});
    }
    else if ((source & 0x1F) == 0)
    {
        // Zero - don't shift
        setPSW (ConditionCodes {.V = false, .C = false});
        tmp = destination;
    }
    else
    {
        // Positive - shift left
        s16 mask = 0;
        // Note that the bitwise and/assignment operator can't be used on
        // CondData objects.
        source = source & 0x1F;
        tmp = destination << source;
        if (source > 0)
        {
            mask = 0x8000;
            mask >>= source;
            u16 tmp2 = destination & mask;
            setPSW (ConditionCodes {.V = !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF))});
        }
        else
        {
            setPSW (ConditionCodes {.V = false});
        }
        setPSW (ConditionCodes {.C = (bool) ((destination << (source - 1)) & 0x8000)});
        if ((destination & 0x8000) != (tmp & 0x8000))
        {
            setPSW (ConditionCodes {.V = true});
        }
    }
    registers[regNr] = tmp;
    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000), .Z = tmp == 0});

    return true;
}
#endif // _ASH_H_