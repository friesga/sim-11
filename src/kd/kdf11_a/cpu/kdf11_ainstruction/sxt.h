#ifndef _SXT_H_
#define _SXT_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// SXT - sign extend
//
// Operation:
//  (dst) <- 0 if N bit is clear
//  (dst) <- 1 if N bit is set
//
// Condition Codes:
//  N: unaffected
//  Z: set if N bit clear
//  V: cleared
//  C: unaffected
//
// If the condition code bit N is set then a -1 is placed in the destination
// operand: if N bit is clear, then a 0 is placed in the destination operand.
//
class KDF11_AInstruction::SXT : public SingleOperandInstruction, public WithFactory<SXT>
{
public:
    SXT (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::SXT::SXT (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::SXT::execute ()
{
    u16 result = isSet (PSW_N) ? 0177777 : 0;

    setPSW (ConditionCodes {.Z = !isSet (PSW_N),
        .V = false});

    if (!writeOperand (result))
        return false;

    return true;
}

#endif // _SXT_H_