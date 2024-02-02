#ifndef _MOV_H_
#define _MOV_H_

#include "kdf11instruction.h"
#include "kd/kdf11/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// MOV - move source to destination
//
// Operation:
//  (dst) <- (src)
//
// Condition Codes:
//  N: set if (src) < 0: cleared otherwise
//  Z: set if (src) = 0: cleared otherwise
//  V: cleared
//  C: not affected
//
// Moves the source operand to the destination location. The previous
// contents of the destination are lost. The contents of the source address
// are not affected.
// 
// This class implements the MOV instruction for the KDF11-A. 
//
class KDF11Instruction::MOV : public KD11DoubleOperandInstruction, public WithFactory<MOV>
{
public:
    MOV (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::MOV::MOV (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}


inline bool KDF11Instruction::MOV::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source))
        return false;

    setPSW (ConditionCodes ({.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false}));
    
    if (!writeDestinationOperand (source.value ()))
        return false;

    return true;
}

#endif // _MOV_H_