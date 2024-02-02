#ifndef _BIS_H_
#define _BIS_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11/kdf11_a/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// BIS - bit set
//
// Operation:
//  (dst) <- (src) v (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set. cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs "Inclusive OR"operation between the source and destination
// operands and leaves the result at the destination address: that is,
// corresponding bits set in the source are set in the destination. The
// contents of the destination are lost.
//
class KDF11_AInstruction::BIS : public KD11DoubleOperandInstruction, public WithFactory<BIS>
{
public:
    BIS (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::BIS::BIS (CpuData* cpuData,
        CpuControl* cpuControl, MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::BIS::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return false;

    u16 tmp = source | destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    if (!writeDestinationOperand (tmp))
        return false;

    return true;
}

#endif // _BIS_H_