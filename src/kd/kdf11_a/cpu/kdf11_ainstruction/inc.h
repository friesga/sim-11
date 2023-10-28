#ifndef _INC_H_
#define _INC_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common//instructions/withfactory.h"

// INC - increment destination
//
// Operation:
//  (dst) <- (dst) + 1
//
// Condition Codes:
//  N: set if result is <0; cleared otherwise
//  Z: set if result is O; cleared otherwise
//  V: set if (dst) held 077777; cleared otherwise
//  C: not affected
//
// Add one to contents of destination
//
class KDF11_AInstruction::INC : public SingleOperandInstruction, public WithFactory<INC>
{
public:
    INC (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::INC::INC (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::INC::execute ()
{
    CondData<u16> contents;
    if (!readOperand (&contents))
        return false;

    // Increment the operand and write it to the operand location
    u16 result = contents + 1;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = contents == 077777});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _INC_H_