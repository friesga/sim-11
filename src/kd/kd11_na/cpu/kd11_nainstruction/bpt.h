#ifndef _BPT_H_
#define _BPT_H_

#include "kd11_nainstruction.h"
#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"
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
class KD11_NAInstruction::BPT : public NoOperandInstruction, public WithFactory<BPT>
{
public:
    BPT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::BPT::BPT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::BPT::execute ()
{
    return CpuData::Trap::BreakpointTrap;
}

#endif // _BPT_H_