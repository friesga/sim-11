#ifndef _TRAP_H_
#define _TRAP_H_

#include "commoninstruction.h"
#include "kd/common/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// TRAP - trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (34)
//  PS <- (36)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector 
//
// Operation codes from 104400 to 104777 are TRAP instructions. TRAPs and EMTs
// are identical in operation, except that the trap vector for TRAP is at
// address 34.
//
class CommonInstruction::TRAP : public NoOperandInstruction, public WithFactory<TRAP>
{
public:
    TRAP (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::TRAP::TRAP (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    NoOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::TRAP::execute ()
{
    cpuData_->setTrap (CpuData::TrapCondition::TrapInstruction);
    return true;
}

#endif // _TRAP_H_