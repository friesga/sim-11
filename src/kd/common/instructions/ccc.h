#ifndef _CCC_H_
#define _CCC_H_

#include "commoninstruction.h"
#include "kd/common/ccinstruction/ccinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

//
// Clear condition code bits. Selectable combinations of these bits may
// be cleared together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are cleared.
// 
// The operation code 0240 is documented as the NOP (no-operation)
// instruction. The operation code indicates this a CCC instruction without
// any condition code set and thus the instruction has no effect.
// 
class CommonInstruction::CCC : public CcInstruction, public WithFactory<CCC>
{
public:
    CCC (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::CCC::CCC (CpuData* cpu, u16 instruction)
    :
    CcInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::CCC::execute ()
{
    cpu_->psw () &= ~getConditionCodes ();
    return CpuData::Trap::None;
}

#endif // _CCC_H_