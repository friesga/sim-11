#ifndef _SCC_H_
#define _SCC_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/ccinstruction/ccinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


//
// Set condition code bits. Selectable combinations of these bits may
// be set together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are set.
//  
class CommonInstruction::SCC : public CcInstruction
{
public:
    SCC (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::SCC::SCC (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    CcInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::SCC::execute ()
{
    cpuData_->psw ().set (PSW::ProtectionMode::ExplicitAccess,
        cpuData_->psw () | getConditionCodes ());
    return true;
}

#endif // _CCC_H_