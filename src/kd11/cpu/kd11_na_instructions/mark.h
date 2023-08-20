#ifndef _MARK_H_
#define _MARK_H_

#include "../markinstruction/markinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"


namespace KD11_NA
{
    class MARK : public MarkInstruction
    {
    public:
        MARK (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    MARK::MARK (CpuData* cpu, u16 instruction)
        :
        MarkInstruction (cpu, instruction)
    {}

    CpuData::Trap MARK::execute ()
    {
        CpuData::GeneralRegisters& registers = cpu_->registers ();

        registers[6] = registers[7] + 2 * numberOfParameters ();
        registers[7] = registers[5];
        registers[5] = cpu_->fetchWord (registers[6]);
        registers[6] += 2;

        return CpuData::Trap::None;
    }
}

#endif // _MARK_H_