#ifndef _RTS_H_
#define _RTS_H_

#include "commoninstruction.h"
#include "kd/common/fisinstruction/fisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    RTS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::RTS::RTS (CpuData* cpu, u16 instruction)
    :
    FISInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::RTS::execute ()
{
    u16 regNr = getRegister ();

    cpu_->registers ()[7] = cpu_->registers ()[regNr];
    cpu_->popWord (&cpu_->registers ()[regNr]);

    return CpuData::Trap::None;
}

#endif // _RTS_H_