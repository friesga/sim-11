#ifndef _MFPD_H_
#define _MFPD_H_

#include "kdf11instruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// MFPD - Move from previous data space
// MFPI - Move from previous instruction space
//
// Operation:
//  (tmp) <- (src)
//  -(SP) <- (tmp)
//
// Condition Codes: 
//  N: set if the source < 0 
//  Z: set if the source = 0 
//  V: cleared 
//  C: unaffected 
//
// Pushes a word onto the current stack from an address in the previous space.
// The source address is computed using the current registers and memory map.
// Since data space does not exist in the KDFll, MFPD executes in the same way
// as an MFPI does. 
//
class KDF11Instruction::MFPD : public SingleOperandInstruction
{
public:
    MFPD (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::MFPD::MFPD (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::MFPD::execute ()
{
    CondData<u16> source;

    // The source operand is determined in the current memory management
    // mode and then retrieved using the previous mode.
    operandLocation_ =  getOperandLocation (cpuData_->registers ());
    source = operandLocation_.prevModeContents<CondData<u16>> ();

    if (!source.hasValue ())
        return false;

    if (!mmu_->pushWord (source))
        return false;

    if (cpuData_->stackOverflow ())
        cpuData_->setTrap (CpuData::TrapCondition::StackOverflow);

    setPSW (ConditionCodes {.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false});

    return true;
}

#endif // _MFPD_H_