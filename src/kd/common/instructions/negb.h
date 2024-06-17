#ifndef _NEGB_H_
#define _NEGB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
class NEGB : public SingleOperandInstruction
{
public:
    NEGB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline NEGB::NEGB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _NEGB_H_