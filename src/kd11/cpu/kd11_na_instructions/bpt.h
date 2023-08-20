#ifndef _BPT_H_
#define _BPT_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class BPT : public NoOperandInstruction
    {
    public:
        BPT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    BPT::BPT (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap BPT::execute ()
    {
        return CpuData::Trap::BreakpointTrap;
    }
}

#endif // _BPT_H_