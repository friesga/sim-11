#ifndef _UNUSED_H_
#define _UNUSED_H_

#include "kd11_nainstruction.h"
#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"
#include "../../../trace/trace.h"
#include "withfactory.h"

// Unused/reserved instruction
//
// Reserved instructions trap to vector address 10.
//
class KD11_NAInstruction::Unused : public NoOperandInstruction, public WithFactory<Unused>
{
public:
    Unused (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

// The two function members are defined inline as this header file is
// included in both opcodetables.cpp and kd11_na.cpp.
inline KD11_NAInstruction::Unused::Unused (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KD11_NAInstruction::Unused::execute ()
{
    trace.trap (TrapRecordType::TRAP_ILL, 010);
    return CpuData::Trap::IllegalInstructionTrap;
}

#endif // _UNUSED_H_