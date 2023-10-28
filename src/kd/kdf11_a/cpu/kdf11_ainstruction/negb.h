#ifndef _NEGB_H_
#define _NEGB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
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
class KDF11_AInstruction::NEGB : public SingleOperandInstruction, public WithFactory<NEGB>
{
public:
    NEGB (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::NEGB::NEGB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::NEGB::execute ()
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