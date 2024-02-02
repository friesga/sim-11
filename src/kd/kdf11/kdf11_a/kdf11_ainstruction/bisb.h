#ifndef _BISB_H_
#define _BISB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11/kdf11_a/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
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
    BISB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::BISB::BISB (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::BISB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return false;

    u8 tmp = source | destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    if (!writeDestinationOperand (tmp))
        return false;

    return true;
}

#endif // _BISB_H_