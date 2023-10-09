#ifndef _BPT_H_
#define _BPT_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    CpuData::Trap execute () override;
};

CommonInstruction::BPT::BPT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::BPT::execute ()
{
    return CpuData::Trap::BreakpointTrap;
}

#endif // _BPT_H_