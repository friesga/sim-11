#ifndef _HALT_H_
#define _HALT_H_

#include "kd11_na.h"
#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// HALT - halt
// 
// Condition Codes: not affected
// 
// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
//
class KD11_NAInstruction::HALT : public NoOperandInstruction, public WithFactory<HALT>
{
public:
    HALT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::HALT::HALT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::HALT::execute ()
{
    cpu_->halt ();
    return CpuData::Trap::None;
}

#endif // _HALT_H_