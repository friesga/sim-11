#ifndef _TST_H_
#define _TST_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// TST - test destination
//
// Operation
// (dst) <- (dst)
//
// Condition Codes:
//  N: set if the result is < 0; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: cleared
//
class CommonInstruction::TST : public SingleOperandInstruction, public WithFactory<TST>
{
public:
    TST (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::TST::TST (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::TST::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    setPSW (ConditionCodes {.N = (bool) (contents & 0100000),
        .Z = contents == 0,
        .V = false,
        .C = false});

    return true;
}

#endif // _TST_H_