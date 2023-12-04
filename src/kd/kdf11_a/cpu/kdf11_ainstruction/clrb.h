#ifndef _CLRB_H_
#define _CLRB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/clrb.h"

// CLRB - clear destination byte
//
// Operation:
//  refer to CLR
// 
// Condition Codes:
//  refer to CLR
//
// This class implements the CLRB instruction for the KDF11-A. See comment
// in KDF11_AInstruction::MOV.
//
class KDF11_AInstruction::CLRB : public SingleOperandInstruction, public WithFactory<CLRB>
{
public:
    CLRB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::CLRB::CLRB (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::CLRB::execute ()
{
    setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    if (!writeOperand ((u8) 0))
        return false;

    return true;
}

#endif // _CLRB_H_