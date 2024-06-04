#ifndef _NEGB_H_
#define _NEGB_H_

#include "kdf11instruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// NEGB - negate destination byte
//
// Operation:
//  refer to NEG
// 
// Condition Codes:
//  refer to NEG
//
class KDF11Instruction::NEGB : public SingleOperandInstruction, public WithFactory<NEGB>
{
public:
    NEGB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::NEGB::NEGB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::NEGB::execute ()
{
    CondData<u8> operand;
    if (!readOperand (&operand))
        return false;

    if (operand != 0200)
        operand = -operand;

    setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = operand == 0,
        .V = operand == 0200,
        .C = operand != 0});

    if (!writeOperand (operand.value ()))
        return false;

    return true;
}

#endif // _NEGB_H_