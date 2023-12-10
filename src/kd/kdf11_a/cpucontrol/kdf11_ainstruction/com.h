#ifndef _COM_H_
#define _COM_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// COM - complement destination
//
// Operation:
//  (dst) <- ~(dst)
//
// Condition Codes:
//  N: set if most significant bit of result is set; cleared otherwise
//  Z: set if result is 0; cleared otherwise
//  V: cleared
//  C: set
//
// Replaces the contents of the destination address by their logical
// complement (each bit equal to 0 is set and each bit equal to 1 is cleared).
//
class KDF11_AInstruction::COM : public SingleOperandInstruction, public WithFactory<COM>
{
public:
    COM (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::COM::COM (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::COM::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return false;

    // Complement the operand, set the condition codes accordingly and
    // write it to the operand location.
    operand = ~operand;

    setPSW ({ConditionCodes {.N = (bool) (operand & 0x8000),
        .Z = operand == 0,
        .V = false,
        .C = true}});

    if (!writeOperand (operand.value ()))
        return false;

    return true;
}

#endif // _COM_H_