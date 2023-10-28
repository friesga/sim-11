#ifndef _INCB_H_
#define _INCB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// INCB - increment destination byte
//
// Operation:
//  refer to INC
// 
// Condition Codes:
//  refer to INC
//
class CommonInstruction::INCB : public SingleOperandInstruction, public WithFactory<INCB>
{
public:
    INCB (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::INCB::INCB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::INCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u8 result = (u8)(source + 1);

    if (!writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x80),
        .Z = result == 0,
        .V = source == 000177});

    return true;
}

#endif // _INCB_H_