#ifndef _IOT_H_
#define _IOT_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/nooperandinstruction/nooperandinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/cpucontrol.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// IOT - input/output trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (20)
//  PS <- (22)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
class CommonInstruction::IOT : public NoOperandInstruction, public WithFactory<IOT>
{
public:
    IOT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::IOT::IOT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::IOT::execute ()
{
    return CpuData::Trap::InputOutputTrap;
}

#endif // _IOT_H_