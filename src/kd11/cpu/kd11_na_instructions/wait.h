#ifndef _WAIT_H_
#define _WAIT_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

// WAIT - wait for interrupt
//
// Condition Codes: not affected
//
// Provides a way for the processor to relinquish use of the bus while it
// waits for an external interrupt request. Having been given a WAIT command,
// the processor will not compete for bus use by fetching instructions or
// operands from memory.
//
// In WAIT, as in all instructions, the PC points to the next instruction
// following the WAIT instruction. Thus when an interrupt causes the PC and PS
// to be pushed onto the processor stack, the address of the next instruction
// following the WAIT is saved. The exit from the interrupt routine (i.e.
// execution of an RTI instruction) will cause resumption of the interrupted
// process at the instruction following the WAIT.
//
namespace KD11_NA
{
    class WAIT : public NoOperandInstruction
    {
    public:
        WAIT (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    WAIT::WAIT (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap WAIT::execute ()
    {
        cpu_->wait ();
        return CpuData::Trap::None;
    }
}

#endif // _WAIT_H_