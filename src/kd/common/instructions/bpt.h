#ifndef _BPT_H_
#define _BPT_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// BPT - breakpoint trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (14)
//  PS <- (16)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
class CommonInstruction::BPT : public NoOperandInstruction, public WithFactory<BPT>
{
public:
    BPT (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::BPT::BPT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::BPT::execute ()
{
    cpu_->setTrap (CpuData::Trap::BreakpointTrap);
    return true;
}

#endif // _BPT_H_