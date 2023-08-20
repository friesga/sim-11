#ifndef _IOT_H_
#define _IOT_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

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
namespace KD11_NA
{
    class IOT : public NoOperandInstruction
    {
    public:
        IOT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    IOT::IOT (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap IOT::execute ()
    {
        return CpuData::Trap::InputOutputTrap;
    }
}

#endif // _IOT_H_