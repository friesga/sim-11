#ifndef _BITB_H_
#define _BITB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
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
    BITB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline  KDF11_AInstruction::BITB::BITB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool  KDF11_AInstruction::BITB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

#endif // _BITB_H_