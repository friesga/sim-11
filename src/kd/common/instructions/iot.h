#ifndef _IOT_H_
#define _IOT_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
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
    bool execute () override;
};

inline CommonInstruction::IOT::IOT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::IOT::execute ()
{
    cpu_->setTrap (CpuData::TrapCondition::InputOutputTrap);
    return true;
}

#endif // _IOT_H_