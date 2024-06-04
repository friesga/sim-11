#ifndef _CCC_H_
#define _CCC_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/ccinstruction/ccinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
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
    CCC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::CCC::CCC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    CcInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::CCC::execute ()
{
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () & ~getConditionCodes ());
    return true;
}

#endif // _CCC_H_