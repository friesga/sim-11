#ifndef _SCC_H_
#define _SCC_H_

#include "../ccinstruction/ccinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

//
// Set condition code bits. Selectable combinations of these bits may
// be set together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are set.
//  
namespace KD11_NA
{
    class SCC : public CcInstruction
    {
    public:
        SCC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    SCC::SCC (CpuData* cpu, u16 instruction)
        :
        CcInstruction (cpu, instruction)
    {}

    CpuData::Trap SCC::execute ()
    {
        cpu_->psw () |= getConditionCodes ();
        return CpuData::Trap::None;
    }
}

#endif // _CCC_H_