#ifndef _SCC_H_
#define _SCC_H_

#include "kd11_na.h"
#include "../ccinstruction/ccinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

//
// Set condition code bits. Selectable combinations of these bits may
// be set together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are set.
//  
class KD11_NAInstruction::SCC : public CcInstruction, public WithFactory<SCC>
{
public:
    SCC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::SCC::SCC (CpuData* cpu, u16 instruction)
    :
    CcInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::SCC::execute ()
{
    cpu_->psw () |= getConditionCodes ();
    return CpuData::Trap::None;
}

#endif // _CCC_H_