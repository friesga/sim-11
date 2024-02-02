#ifndef _SBC_H_
#define _SBC_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// SBC - subtract carry
//
// Operation
//  (dst) <- (dst) - (C)
//
// Condition Codes:
//  N: set if result <O; cleared otherwise
//  Z: set if result O; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: set if (dst) was 0 and C was 1; cleared otherwise
//
// Subtracts the contents of the C-bit from the destination. This permits the
// carry from the subtraction of two low-order words to be subtracted from the
// high order part of the result.
//
class KDF11_AInstruction::SBC : public SingleOperandInstruction, public WithFactory<SBC>
{
public:
    SBC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::SBC::SBC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::SBC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u16 result = contents - cBit;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 0100000,
        .C = contents == 0 && cBit});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _SBC_H_