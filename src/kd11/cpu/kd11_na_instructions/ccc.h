#ifndef _CCC_H_
#define _CCC_H_

#include "../ccinstruction/ccinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

//
// Clear condition code bits. Selectable combinations of these bits may
// be cleared together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are cleared.
// 
namespace KD11_NA
{
    class CCC : public CcInstruction
    {
    public:
        CCC (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    CCC::CCC (CpuData* cpu, u16 instruction)
        :
        CcInstruction (cpu, instruction)
    {}

    CpuData::Trap CCC::execute ()
    {
        cpu_->psw () &= ~getConditionCodes ();
        return CpuData::Trap::None;
    }
}

#endif // _CCC_H_