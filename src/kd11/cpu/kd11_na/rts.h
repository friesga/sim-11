#ifndef _RTS_H_
#define _RTS_H_

#include "kd11_na.h"
#include "../fisinstruction/fisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NA::RTS : public FISInstruction, public WithFactory<RTS>
{
public:
    RTS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::RTS::RTS (CpuData* cpu, u16 instruction)
    :
    FISInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::RTS::execute ()
{
    u16 regNr = getRegister ();

    cpu_->registers ()[7] = cpu_->registers ()[regNr];
    cpu_->popWord (&cpu_->registers ()[regNr]);

    return CpuData::Trap::None;
}

#endif // _RTS_H_