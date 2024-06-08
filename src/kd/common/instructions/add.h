#ifndef _ADD_H_
#define _ADD_H_

#include "commoninstruction.h"
#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ADD - add source to destination
//
// Operation:
//  (dst) <- (src) + (dst)
//
// Condition Codes:
//  N: set if result <0; cleared otherwise
//  Z: set if result = O; cleared otherwise
//  V: set if there was arithmetic overflow as a result of the operation;
//     that is both operands were of the same sign and the result was of the
//     opposite sign; cleared otherwise
//  C: set if there was a carry from the most significant bit of the result;
//     cleared otherwise
//
// Adds the source operand to the destinatign operand and stores the result at
// the destination address. The original contents of the destination are lost.
// The contents of the source are not affected. Two's complement addition is
// performed.
//
class CommonInstruction::ADD : public DoubleOperandInstruction
{
public:
    ADD (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::ADD::ADD (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

// ToDo: To be removed
inline bool CommonInstruction::ADD::execute ()
{
    throw "Should not happen";
}

#endif // _ADD_H_