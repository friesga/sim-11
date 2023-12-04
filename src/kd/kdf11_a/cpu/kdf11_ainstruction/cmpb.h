#ifndef _CMPB_H_
#define _CMPB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11_a/cpu/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// CMPB - compare source to destination byte
// 
// Operation:
//  refer to CMP
// 
// Condition Codes:
//  refer to CMP
//
class KDF11_AInstruction::CMPB : public KD11DoubleOperandInstruction, public WithFactory<CMPB>
{
public:
    CMPB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::CMPB::CMPB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::CMPB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return false;

    u16 tmp = (u8) (source - destination);

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = ((source & 0x80) != (destination & 0x80)) && ((destination & 0x80) == (tmp & 0x80)),
        .C = (bool) ((source - destination) & 0x100)});

    return true;
}

#endif // _CMPB_H_