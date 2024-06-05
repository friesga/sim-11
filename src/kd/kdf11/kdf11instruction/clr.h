#ifndef _CLR_H_
#define _CLR_H_

#include "kdf11instruction.h"
#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// CLR - clear destination
//
// Operation:
//  (dst) <- 0
//
// Condition Codes:
//  N: cleared
//  Z: set
//  V: cleared
//  C: cleared
//
// Contents of specified destination are replaced with zeroes.
//
// This class implements the CLR instruction for the KDF11-A. This processor
// allows access to the PSW via address 0177776. As JKDBD0 test 243 shows,
// a CLR of the PSW does not set the condition codes. Therefore the condition
// codes are set before the CLR is executed. As a consequence the original
// conditions codes have to be restored when clearance of the destionation
// operand location fails. The setPSW() can be used for this purpose as the
// T-bit is not affected by an Explicit PS Access (see EK-KDF-UG-PR2,
// Table 8-1).
//
class KDF11Instruction::CLR : public SingleOperandInstruction
{
public:
    CLR (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::CLR::CLR (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::CLR::execute ()
{
    setPSW (ConditionCodes {.N = false,
        .Z = true,
        .V = false,
        .C = false});

    if (!writeOperand ((u16) 0))
        return false;

    return true;
}

#endif // _CLR_H_