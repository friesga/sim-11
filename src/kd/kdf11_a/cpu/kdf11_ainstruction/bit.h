#ifndef _BIT_H_
#define _BIT_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11_a/cpu/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// BIT - bit test
//
// Operation:
//  (src) ^ (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs logical "and"comparison of the source and destination operands
// and modifies condition codes accordingly. Neither the source nor
// destination operands are affected.
//
class KDF11_AInstruction::BIT : public KD11DoubleOperandInstruction, public WithFactory<BIT>
{
public:
    BIT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::BIT::BIT (CpuData* cpu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::BIT::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return CpuData::Trap::None;
}

#endif // _BIT_H_