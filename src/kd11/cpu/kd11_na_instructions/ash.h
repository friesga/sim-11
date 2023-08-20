#ifndef _ASH_H_
#define _ASH_H_

#include "../eisinstruction/eisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class ASH : public EisInstruction
    {
    public:
        ASH (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    ASH::ASH (CpuData* cpu, u16 instruction)
        :
        EisInstruction (cpu, instruction)
    {}

    CpuData::Trap ASH::execute ()
    {
        u16 regNr = getRegisterNr ();
        u16 tmp {0};
        CpuData::GeneralRegisters& registers = cpu_->registers ();
        u16 destination = registers[regNr];

        CondData<u16> source;
        if (!readOperand (&source))
            return CpuData::Trap::BusError;

        if (source & 0x20)
        { 
            // Negative - shift right
            s16 stmp, stmp2;
            source = (~source & 0x1F) + 1;
            stmp = (s16) destination;
            stmp2 = stmp >> (source - 1);
            stmp >>= source;
            tmp = (u16) stmp;
            setConditionCodeIf_ClearElse (PSW_C, stmp2 & 1);
            clearConditionCode (PSW_V);
        }
        else if ((source & 0x1F) == 0)
        {
            // Zero - don't shift
            clearConditionCode (PSW_V);
            clearConditionCode (PSW_C);
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
                setConditionCodeIf_ClearElse (PSW_V, !((tmp2 == 0) || (((tmp2 & mask) | ~mask) == 0xFFFF)));
            }
            else
            {
                clearConditionCode (PSW_V);
            }
            setConditionCodeIf_ClearElse (PSW_C, (destination << (source - 1)) & 0x8000);
            if ((destination & 0x8000) != (tmp & 0x8000))
            {
                setConditionCode (PSW_V);
            }
        }
        registers[regNr] = tmp;
        setConditionCodeIf_ClearElse (PSW_N, tmp & 0x8000);
        setConditionCodeIf_ClearElse (PSW_Z, !tmp);

        return CpuData::Trap::None;
    }
}

#endif // _ASH_H_