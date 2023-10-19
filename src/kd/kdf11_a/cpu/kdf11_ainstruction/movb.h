#ifndef _MOVB_H_
#define _MOVB_H_

#include "kdf11_ainstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// MOVB - move source to destination
// 
// Operation:
//  refer to MOV
// 
// Condition Codes:
//  refer to MOV
//
// The MOVB to a register (unique among byte instructions) extends the most
// significant bit of the low order byte (sign extension). Otherwise MOVB
// operates on bytes exactly as MOV operates on words.
//
// This class implements the MOVB instruction for the KDF11-A. See comment
// in KDF11_AInstruction::MOV.
//
class KDF11_AInstruction::MOVB : public DoubleOperandInstruction, public WithFactory<MOVB>
{
public:
    MOVB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline KDF11_AInstruction::MOVB::MOVB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap KDF11_AInstruction::MOVB::execute ()
{
    CondData<u8> source;

    if (!readSourceOperand (&source))
        return CpuData::Trap::BusError;

    // Make the source a signed eight bit value 
    s8 tmp = (s8) source;
    u16 originalPsw = cpu_->pswValue ();

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value s8 is directly written into
    // the register, causing sign extension in the register.
    OperandLocation destOperandLocation =
        getDestinationOperandLocation (cpu_->registers ());

    if (destOperandLocation.isA<RegisterOperandLocation> ())
        cpu_->registers ()[destOperandLocation] = tmp;
    else
        if (!destOperandLocation.write<u8> (tmp))
        {
            cpu_->setPSW (originalPsw);
            return CpuData::Trap::BusError;
        }

    return CpuData::Trap::None;
}

#endif // _MOVB_H_