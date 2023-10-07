#ifndef _HALT_H_
#define _HALT_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/nooperandinstruction/nooperandinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/cpucontrol.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// HALT - halt
// 
// Condition Codes: not affected
// 
// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
//
class CommonInstruction::HALT : public NoOperandInstruction, public WithFactory<HALT>
{
public:
    HALT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::HALT::HALT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::HALT::execute ()
{
    cpu_->halt ();
    return CpuData::Trap::None;
}

#endif // _HALT_H_