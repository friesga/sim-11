#ifndef _SUB_H_
#define _SUB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11_a/cpu/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// SUB - subtract src from dst
//
// Operation:
//  (dst) <- (dst) - (src)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation,
//     that is if operands were of opposite signs and the sign of the source
//     was the same as the sign of the result; cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
class KDF11_AInstruction::SUB : public KD11DoubleOperandInstruction, public WithFactory<SUB>
{
public:
    SUB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::SUB::SUB (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::SUB::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return false;

    u16 result = destination - source;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) &&
             ((source & 0x8000) == (result & 0x8000)),
        .C = (bool) (((u32) destination - (u32) source) & 0x10000)});

    if (!writeDestinationOperand (result))
        return false;

    return true;
}

#endif // _SUB_H_