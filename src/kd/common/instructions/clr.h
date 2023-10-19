#ifndef _CLR_H_
#define _CLR_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
class CommonInstruction::CLR : public SingleOperandInstruction, public WithFactory<CLR>
{
public:
    CLR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::CLR::CLR (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::CLR::execute ()
{
    if (!writeOperand ((u16)0))
        return CpuData::Trap::BusError;

    clearConditionCode (PSW_N | PSW_V | PSW_C);
    setConditionCode (PSW_Z);

    return CpuData::Trap::None;
}

#endif // _CLR_H_