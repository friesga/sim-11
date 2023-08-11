#ifndef _RTS_H_
#define _RTS_H_

#include "../fisinstruction/fisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// RTS - Return from subroutine
// 
// Operation:
//  PC <- (reg)
//  (reg) <- (SP)^
// 
// Loads contents of reg into PC and pops the top element of
// the processor stack into the specified register.
//
namespace KD11_NA
{
    class RTS : public FISInstruction
    {
    public:
        RTS (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    RTS::RTS (CpuData* cpu, u16 instruction)
        :
        FISInstruction (cpu, instruction)
    {}

    CpuData::Trap RTS::execute ()
    {
        u16 regNr = getRegister ();

        cpu_->registers ()[7] = cpu_->registers ()[regNr];
        cpu_->popWord (&cpu_->registers ()[regNr]);

        return CpuData::Trap::None;
    }
}

#endif // _RTS_H_