#ifndef _TST_H_
#define _TST_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class TST : public SingleOperandInstruction
{
public:
    TST (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline TST::TST (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool TST::execute ()
{
    throw "Should not happen";
}

#endif // _TST_H_