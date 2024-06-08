#ifndef _WAIT_H_
#define _WAIT_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class CommonInstruction::WAIT : public NoOperandInstruction
{
public:
    WAIT (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::WAIT::WAIT (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::WAIT::execute ()
{
    throw "Should not happen";
}

#endif // _WAIT_H_