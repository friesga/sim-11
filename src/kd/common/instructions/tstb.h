#ifndef _TSTB_H_
#define _TSTB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// TSTB - test destination byte
//
// Operation:
//  refer to TST
// 
// Condition Codes:
//  refer to TST
//
class CommonInstruction::TSTB : public SingleOperandInstruction, public WithFactory<TSTB>
{
public:
    TSTB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::TSTB::TSTB (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::TSTB::execute ()
{
    CondData<u8> source;
    if (!readOperand (&source))
        return CpuData::Trap::BusError;

    setPSW (ConditionCodes {.N = (bool) (source & 0x80),
        .Z = source == 0,
        .V = false,
        .C = false});

    return CpuData::Trap::None;
}

#endif // _TSTB_H_