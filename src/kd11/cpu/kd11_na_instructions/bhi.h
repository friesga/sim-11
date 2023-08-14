#ifndef _BHI_H_
#define _BHI_H_

#include "../branchinstruction/branchinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"


namespace KD11_NA
{
    class BHI : public BranchInstruction
    {
    public:
        BHI (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BHI::BHI (CpuData* cpu, u16 instruction)
        :
        BranchInstruction (cpu, instruction)
    {}

    CpuData::Trap BHI::execute ()
    {
        executeBranchIf (!isSet (PSW_C) && !isSet (PSW_Z));
        return CpuData::Trap::None;
    }
}

#endif // _BHI_H_