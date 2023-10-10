#ifndef _MOV_H_
#define _MOV_H_

#include "kdf11_ainstruction.h"
#include "kd/kd11_na/cpu/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
// This class implements the KDF11-A version of the MOV instruction. On a
// KDF11-A (and some other processors) the instruction "MOV Rx, (Rx)+"
// using the same register as both register and destination, the contents of
// Rx are incremented by 2 before being used as the source operand. This
// implies the destination operand location is determined before the source
// operand is retrieved.
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

    destinationOperandLocation_ = 
			getDestinationOperandLocation (cpu_->registers ());

    if (!readSourceOperand (&source) ||
        !writeDestinationOperand (source.value ()))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, source & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !source);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _MOV_H_