#ifndef _NEG_H_
#define _NEG_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// NEG - negate destination
//
// Operation:
//  (dst) <- -(dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is O: cleared otherwise
//  V: set if the result is 100000; cleared otherwise
//  C: cleared if the result is O; set otherwise
//
// Replaces the contents of the destination address by its two's complement.
// Note that 100000 is replaced by itself (in two's complement notation the
// most negative number has no positive counterpart).
//
class CommonInstruction::NEG : public SingleOperandInstruction, public WithFactory<NEG>
{
public:
    NEG (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::NEG::NEG (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::NEG::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return false;

    // Negate the operand and write it to the operand location
    if (operand != 0100000)
        operand = -operand;

    if (!writeOperand (operand.value ()))
        return false;

    setPSW (ConditionCodes {.N = (bool) (operand & 0100000),
        .Z = operand == 0,
        .V = operand == 0100000,
        .C = operand != 0});

    return true;
}

#endif // _NEG_H_