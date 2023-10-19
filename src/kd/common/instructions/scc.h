#ifndef _SCC_H_
#define _SCC_H_

#include "commoninstruction.h"
#include "kd/common/ccinstruction/ccinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

//
// Set condition code bits. Selectable combinations of these bits may
// be set together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are set.
//  
class CommonInstruction::SCC : public CcInstruction, public WithFactory<SCC>
{
public:
    SCC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::SCC::SCC (CpuData* cpu, u16 instruction)
    :
    CcInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::SCC::execute ()
{
    cpu_->psw () |= getConditionCodes ();
    return CpuData::Trap::None;
}

#endif // _CCC_H_