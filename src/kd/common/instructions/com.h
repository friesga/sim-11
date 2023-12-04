#ifndef _COM_H_
#define _COM_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::COM : public SingleOperandInstruction, public WithFactory<COM>
{
public:
    COM (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::COM::COM (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::COM::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return false;

    // Complement the operand and write it to the operand location
    operand = ~operand;
    if (!writeOperand (operand.value ()))
        return false;

    setPSW ({ConditionCodes {.N = (bool) (operand & 0x8000),
        .Z = operand == 0,
        .V = false,
        .C = true}});

    return true;
}

#endif // _COM_H_