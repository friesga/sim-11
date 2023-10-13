#ifndef _MOV_H_
#define _MOV_H_

#include "kd/kdf11_a/cpu/kdf11_ainstruction/kdf11_ainstruction.h"
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
// This class implements the MOV instruction for the KDF11-A. This processor
// allows access to the PSW via address 0177776. As JKDBD0 test 33 shows,
// a MOV to the PSW does not set the condition codes. Therefore the condition
// codes are set before the MOV is executed. As a consequence the original
// conditions codes have to be restored when a write to the destionation
// operand location fails. The setPSW() can be used for this purpose as the
// T-bit is not affected by an Explicit PS Access (see EK-KDF-UG-PR2,
// Table 8-1).
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

    u16 originalPsw = cpu_->pswValue ();

    setConditionCodeIf_ClearElse (PSW_N, source & 0100000);
    setConditionCodeIf_ClearElse (PSW_Z, !source);
    clearConditionCode (PSW_V);
    
    if (!writeDestinationOperand (source.value ()))
    {
        cpu_->setPSW (originalPsw);
        return CpuData::Trap::BusError;
    }

    return CpuData::Trap::None;
}

#endif // _MOV_H_