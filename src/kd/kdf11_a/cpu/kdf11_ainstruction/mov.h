#ifndef _MOV_H_
#define _MOV_H_

#include "kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
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
class KDF11_AInstruction::MOV : public DoubleOperandInstruction, public WithFactory<MOV>
{
public:
    MOV (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::MOV::MOV (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}


inline CpuData::Trap KDF11_AInstruction::MOV::execute ()
{
    CondData<u16> source, destination;

    if (!readSourceOperand (&source))
        return CpuData::Trap::BusError;

    setPSW (ConditionCodes ({.N = (bool) (source & 0100000),
        .Z = source == 0,
        .V = false}));
    
    if (!writeDestinationOperand (source.value ()))
        return CpuData::Trap::BusError;

    return CpuData::Trap::None;
}

#endif // _MOV_H_