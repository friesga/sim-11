#ifndef _UNUSED_H_
#define _UNUSED_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

// Unused/reserved instruction
//
// Reserved instructions trap to vector address 10.
//
namespace KD11_NA
{
    class Unused : public NoOperandInstruction
    {
    public:
        Unused (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    Unused::Unused (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap Unused::execute ()
    {
        trace.trap (TrapRecordType::TRAP_ILL, 010);
        return CpuData::Trap::IllegalInstructionTrap;
    }
}

#endif // _UNUSED_H_