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

    private:
        CpuData* cpu_;
        u16 instruction_;
    };

    RTS::RTS (CpuData* cpu, u16 instruction)
        :
        FISInstruction (cpu, instruction),
        cpu_ {cpu},
        instruction_ {instruction}
    {}

    CpuData::Trap RTS::execute ()
    {
        FISInstruction rtsInstruction (cpu_, instruction_);
        u16 regNr = rtsInstruction.getRegister ();

        cpu_->registers ()[7] = cpu_->registers ()[regNr];
        cpu_->popWord (&cpu_->registers ()[regNr]);

        return CpuData::Trap::None;
    }
}

#endif // _RTS_H_