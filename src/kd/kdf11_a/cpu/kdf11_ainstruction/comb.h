#ifndef _COMB_H_
#define _COMB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
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
class KDF11_AInstruction::COMB : public SingleOperandInstruction, public WithFactory<COMB>
{
public:
    COMB (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::COMB::COMB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::COMB::execute ()
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