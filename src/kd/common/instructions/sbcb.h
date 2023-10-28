#ifndef _SBCB_H_
#define _SBCB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// SBCB - subtract carry byte
//
// Operation:
//  refer to SBC
// 
// Condition Codes:
//  refer to SBC
//
class CommonInstruction::SBCB : public SingleOperandInstruction, public WithFactory<SBCB>
{
public:
    SBCB (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::SBCB::SBCB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::SBCB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return false;

    u16 cBit = isSet (PSW_C) ? 1 : 0;
    u8 destination = (u8) (source - cBit);

    if (!writeOperand (destination))
        return false;

    setPSW (ConditionCodes {.N = (bool) (destination & 0x80),
        .Z = destination == 0,
        .V = source == 0200,
        .C = source == 0 && cBit});

    return true;
}

#endif // _SBCB_H_