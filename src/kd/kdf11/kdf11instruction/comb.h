#ifndef _COMB_H_
#define _COMB_H_

#include "kdf11instruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// COMB - complement destination byte
//
// Operation:
//  refer to COM
// 
// Condition Codes:
//  refer to COM
//
class KDF11Instruction::COMB : public SingleOperandInstruction, public WithFactory<COMB>
{
public:
    COMB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::COMB::COMB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::COMB::execute ()
{
    CondData<u8> operand;
    if (!readOperand (&operand))
        return false;

    // Complement the operand, adjust the condition codes and write the
    // complement to the operand location.
    operand = ~operand;

    setPSW ({ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = true}});

    if (!writeOperand (operand.value ()))
        return false;

    return true;
}

#endif // _COMB_H_