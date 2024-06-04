#ifndef _BIT_H_
#define _BIT_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BIT - bit test
//
// Operation:
//  (src) ^ (dst)
//
// Condition Codes:
//  N: set if high-order bit of result set: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: not affected
//
// Performs logical "and"comparison of the source and destination operands
// and modifies condition codes accordingly. Neither the source nor
// destination operands are affected.
//
class CommonInstruction::BIT : public DoubleOperandInstruction, public WithFactory<BIT>
{
public:
    BIT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BIT::BIT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::BIT::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !readDestinationOperand (&destination))
        return false;

    u16 tmp = source & destination;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x8000),
        .Z = tmp == 0,
        .V = false});

    return true;
}

#endif // _BIT_H_