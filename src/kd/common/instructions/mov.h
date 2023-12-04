#ifndef _MOV_H_
#define _MOV_H_

#include "commoninstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// MOV - move source to destination
//
// Operation:
//  (dst) <- (src)
//
// Condition Codes:
//  N: set if (src) < 0: cleared otherwise
//  Z: set if (src) = 0: cleared otherwise
//  V: cleared
//  C: not affected
//
// Moves the source operand to the destination location. The previous
// contents of the destination are lost. The contents of the source address
// are not affected.
//
class CommonInstruction::MOV : public DoubleOperandInstruction, public WithFactory<MOV>
{
public:
    MOV (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::MOV::MOV (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::MOV::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source) ||
        !writeDestinationOperand (source.value ()))
        return false;

    setPSW (ConditionCodes ({.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false}));

    return true;
}

#endif // _MOV_H_