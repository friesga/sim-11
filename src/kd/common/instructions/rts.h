#ifndef _RTS_H_
#define _RTS_H_

#include "commoninstruction.h"
#include "kd/common/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// RTS - Return from subroutine
// 
// Operation:
//  PC <- (reg)
//  (reg) <- (SP)^
// 
// Loads contents of reg into PC and pops the top element of
// the processor stack into the specified register.
//
class CommonInstruction::RTS : public FISInstruction, public WithFactory<RTS>
{
public:
    RTS (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::RTS::RTS (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    FISInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::RTS::execute ()
{
    u16 regNr = getRegister ();

    cpuData_->registers ()[7] = cpuData_->registers ()[regNr];
    mmu_->popWord (&cpuData_->registers ()[regNr]);

    return true;
}

#endif // _RTS_H_