#ifndef _BISB_H_
#define _BISB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11_a/cpu/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// BISB - bit set byte
//
// Operation:
//  refer to BIS
// 
// Condition Codes:
//  refer to BIS
//
class KDF11_AInstruction::BISB : public KD11DoubleOperandInstruction, public WithFactory<BISB>
{
public:
    BISB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::BISB::BISB (CpuData* cpu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::BISB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u8 tmp = source | destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    if (!writeDestinationOperand (tmp))
        return CpuData::Trap::BusError;

    return CpuData::Trap::None;
}

#endif // _BISB_H_