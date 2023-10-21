#ifndef _DEC_H_
#define _DEC_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// DEC - decrement destination
//
// Operation:
//  (dst) <- (dst) - 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: set if (dst) was 100000; cleared otherwise
//  C: not affected
//
// Subtract 1 from the contents of the destination
//
class KDF11_AInstruction::DEC : public SingleOperandInstruction, public WithFactory<DEC>
{
public:
    DEC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::DEC::DEC (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::DEC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return CpuData::Trap::BusError;

    // Increment the operand and write it to the operand location
    u16 result = contents - 1;

    setPSW (ConditionCodes {.N = (bool) (result & 0100000),
        .Z = result == 0,
        .V = contents == 0100000});

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    return CpuData::Trap::None;
}

#endif // _DEC_H_