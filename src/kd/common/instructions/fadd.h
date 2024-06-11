#ifndef _FADD_H_
#define _FADD_H_


#include "kd/common/instructionformats/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// FADD - floating add
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6]+[(R),(R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: cleared
//
// Adds the A argument to the B argument and stores the result in the
// A Argument position on the stack. General register R is used as the stack
// pointer for the operation.
// 
// A <- A + B
//
class FADD : public FISInstruction
{
public:
    FADD (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline FADD::FADD (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    FISInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool FADD::execute ()
{
    throw "Should not happen";
}

#endif // _FADD_H_