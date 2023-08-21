#ifndef _RTT_H_
#define _RTT_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

// RTT - return from interrupt
//
// Operation:
//  PC <- (SP)^
//  PS <- (SP)^
//
// Conditions Codes:
//  N: loaded from processor stack
//  Z: loaded from processor stack
//  V: loaded from processor stack
//  C: loaded from processor stack
//
// Operation is the same as RTI except that it inhibits a trace trap while
// RTI permits trace trap. If new PS has T bit set, trap will occur after
// execution of first instruction after RTT.
//
namespace KD11_NA
{
    class RTT : public NoOperandInstruction
    {
    public:
        RTT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    RTT::RTT (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap RTT::execute ()
    {
        if (!cpu_->popWord (&cpu_->registers ()[7]))
            return CpuData::Trap::BusError;
        if (!cpu_->popWord (&cpu_->psw ()))
            return CpuData::Trap::BusError;

        // Prevent a trace trap on the next instruction
        cpu_->inhibitTraceTrap ();

        return CpuData::Trap::None;
    }
}

#endif // _RTT_H_