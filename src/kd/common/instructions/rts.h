#ifndef _RTS_H_
#define _RTS_H_


#include "kd/common/instructionformats/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// RTS - Return from subroutine
// 
// Operation:
//  PC <- (reg)
//  (reg) <- (SP)^
// 
// Loads contents of reg into PC and pops the top element of
// the processor stack into the specified register.
//
class RTS : public FISInstruction
{
public:
    RTS (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
};

inline RTS::RTS (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    FISInstruction (cpuData, cpuControl, mmu, instruction)
{}

#endif // _RTS_H_