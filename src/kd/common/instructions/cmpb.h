#ifndef _CMPB_H_
#define _CMPB_H_

#include "commoninstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// CMPB - compare source to destination byte
// 
// Operation:
//  refer to CMP
// 
// Condition Codes:
//  refer to CMP
//
class CommonInstruction::CMPB : public DoubleOperandInstruction, public WithFactory<CMPB>
{
public:
    CMPB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::CMPB::CMPB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::CMPB::execute ()
{
    CondData<u8> source, destination;

    if (!readSourceOperand (&source) || !readDestinationOperand (&destination))
        return CpuData::Trap::BusError;

    u16 tmp = (u8) (source - destination);

    setPSW (ConditionCodes {.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = ((source & 0x80) != (destination & 0x80)) && ((destination & 0x80) == (tmp & 0x80)),
        .C = (bool) ((source - destination) & 0x100)});

    return CpuData::Trap::None;
}

#endif // _CMPB_H_