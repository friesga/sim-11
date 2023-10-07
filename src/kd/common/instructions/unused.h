#ifndef _UNUSED_H_
#define _UNUSED_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/nooperandinstruction/nooperandinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/cpucontrol.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "trace/trace.h"
#include "withfactory.h"

// Unused/reserved instruction
//
// Reserved instructions trap to vector address 10.
//
class CommonInstruction::Unused : public NoOperandInstruction, public WithFactory<Unused>
{
public:
    Unused (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

// The two function members are defined inline as this header file is
// included in both opcodetables.cpp and kd11_na.cpp.
inline CommonInstruction::Unused::Unused (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::Unused::execute ()
{
    trace.trap (TrapRecordType::TRAP_ILL, 010);
    return CpuData::Trap::IllegalInstructionTrap;
}

#endif // _UNUSED_H_