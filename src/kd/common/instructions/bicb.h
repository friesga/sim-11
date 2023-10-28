#ifndef _BICB_H_
#define _BICB_H_

#include "commoninstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BICB - bit clear byte
//
// Operation:
//  refer to BIC
// 
// Condition Codes:
//  refer to BIC
//
class CommonInstruction::BICB : public DoubleOperandInstruction, public WithFactory<BICB>
{
public:
    BICB (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BICB::BICB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::BICB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return false;

    u8 tmp = (u8)(~source & destination);

    if (!writeDestinationOperand (tmp))
        return false;

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false});

    return true;
}

#endif // _BICB_H_