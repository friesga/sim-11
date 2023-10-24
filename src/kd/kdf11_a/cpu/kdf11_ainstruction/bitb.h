#ifndef _BITB_H_
#define _BITB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11_a/cpu/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// BITB - bit test byte
//
// Operation:
//  refer to BIT
// 
// Condition Codes:
//  refer to BIT
//
class  KDF11_AInstruction::BITB : public KD11DoubleOperandInstruction, public WithFactory<BITB>
{
public:
    BITB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline  KDF11_AInstruction::BITB::BITB (CpuData* cpu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap  KDF11_AInstruction::BITB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return CpuData::Trap::None;
}

#endif // _BITB_H_