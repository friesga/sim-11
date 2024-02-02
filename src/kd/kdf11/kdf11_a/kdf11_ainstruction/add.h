#ifndef _ADD_H_
#define _ADD_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11/kdf11_a/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// ADD - add source to destination
//
// Operation:
//  (dst) <- (src) + (dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation;
//     that is both operands were of the same sign and the result was of the
//     opposite sign; cleared otherwise
//  C: set if there was a carry from the most significant bit of the result;
//     cleared otherwise
//
// Adds the source operand to the destinatign operand and stores the result at
// the destination address. The original contents of the destination are lost.
// The contents of the source are not affected. Two's complement addition is
// performed.
//
class KDF11_AInstruction::ADD : public KD11DoubleOperandInstruction, public WithFactory<ADD>
{
public:
    ADD (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::ADD::ADD (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::ADD::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return false;

    u16 result = source + destination;

    setPSW (ConditionCodes ({.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = ((source & 0x8000) == (destination & 0x8000)) &&
            ((destination & 0x8000) != (result & 0x8000)),
        .C = (bool) (((u32) source + (u32) destination) & 0x10000)}));

    if (!writeDestinationOperand (result))
        return false;

    return true;
}

#endif // _ADD_H_