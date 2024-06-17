#ifndef _DECB_H_
#define _DECB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// DECB - decrement destination byte
//
// Operation:
//  refer to DEC
// 
// Condition Codes:
//  refer to DEC
//
class DECB : public SingleOperandInstruction
{
public:
    DECB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline DECB::DECB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _DECB_H_