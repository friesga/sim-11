#ifndef _DECB_H_
#define _DECB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
class KDF11_AInstruction::DECB : public SingleOperandInstruction, public WithFactory<DECB>
{
public:
    DECB (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::DECB::DECB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::DECB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u8 result = (u8) (source - 1);

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 0000200});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _DECB_H_