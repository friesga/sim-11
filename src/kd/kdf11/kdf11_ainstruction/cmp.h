#ifndef _CMP_H_
#define _CMP_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// CMP - compare source to destination
//
// Operation:
//  (src)=(dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = 0; cleared otherwise
//  V: set if there was arithmetic overflow; that is. operands were of
//     opposite signs and the sign of the destination was the same as the
//     sign of the result: cleared otherwise
//  C: cleared if there was a carry from the most significant bit of the
//     result; set otherwise
//
// Compares the source and destination operands and sets the condition codes,
// which may then be used for arithmetic and logical conditional branches.
// Both operands are unaffected. The only action is to set the condition codes.
//
class KDF11_AInstruction::CMP : public KD11DoubleOperandInstruction, public WithFactory<CMP>
{
public:
    CMP (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::CMP::CMP (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::CMP::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return false;

    u16 tmp = source - destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = ((source & 0x8000) != (destination & 0x8000)) && ((destination & 0x8000) == (tmp & 0x8000)),
        .C = (bool) (((u32) source - (u32) destination) & 0x10000)});

    return true;
}

#endif // _CMP_H_