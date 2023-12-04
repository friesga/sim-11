#ifndef _NEGB_H_
#define _NEGB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
class CommonInstruction::NEGB : public SingleOperandInstruction, public WithFactory<NEGB>
{
public:
    NEGB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::NEGB::NEGB (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::NEGB::execute ()
{
    CondData<u8> operand;
    if (!readOperand (&operand))
        return false;

    if (operand != 0200)
        operand = -operand;

    if (!writeOperand (operand.value ()))
        return false;

    setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = operand == 0,
        .V = operand == 0200,
        .C = operand != 0});

    return true;
}

#endif // _NEGB_H_