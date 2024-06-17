#ifndef _TSTB_H_
#define _TSTB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// TSTB - test destination byte
//
// Operation:
//  refer to TST
// 
// Condition Codes:
//  refer to TST
//
class TSTB : public SingleOperandInstruction
{
public:
    TSTB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline TSTB::TSTB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _TSTB_H_