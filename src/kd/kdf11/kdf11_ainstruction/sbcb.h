#ifndef _SBCB_H_
#define _SBCB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// SBCB - subtract carry byte
//
// Operation:
//  refer to SBC
// 
// Condition Codes:
//  refer to SBC
//
class KDF11_AInstruction::SBCB : public SingleOperandInstruction, public WithFactory<SBCB>
{
public:
    SBCB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::SBCB::SBCB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::SBCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source - cBit);

    setPSW (ConditionCodes {.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0200,
        .C = source == 0 && cBit});

    if (!writeOperand (destination))
        return false;

    return true;
}

#endif // _SBCB_H_