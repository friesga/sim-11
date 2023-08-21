#ifndef _RESET_H_
#define _RESET_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

// RESET - reset external bus
//
// Condition Codes: not affected
//
// Sends INIT on the BUS for 10 micro seconds. All devices on the BUS are
// reset to their state at power-up. The processor remains in an idle state
// for 90 micro seconds following issuance of INIT.
//
namespace KD11_NA
{
    class RESET : public NoOperandInstruction
    {
    public:
        RESET (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    RESET::RESET (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap RESET::execute ()
    {
        cpu_->busReset ();
        return CpuData::Trap::None;
    }
}

#endif // _RESET_H_