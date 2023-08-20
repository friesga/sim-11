#ifndef _HALT_H_
#define _HALT_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"


namespace KD11_NA
{
    class HALT : public NoOperandInstruction
    {
    public:
        HALT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    HALT::HALT (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap HALT::execute ()
    {
        cpu_->halt ();
        return CpuData::Trap::None;
    }
}

#endif // _HALT_H_