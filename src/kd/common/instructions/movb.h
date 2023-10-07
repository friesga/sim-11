#ifndef _MOVB_H_
#define _MOVB_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::MOVB : public DoubleOperandInstruction, public WithFactory<MOVB>
{
public:
    MOVB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::MOVB::MOVB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::MOVB::execute ()
{
    CondData<u8> source;

    if (!readSourceOperand (&source))
        return CpuData::Trap::BusError;

    s8 tmp = (s8)source;

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value u8 is directly written into
    // the register, causing sign extension in the register.
    OperandLocation destOperandLocation =
        getDestinationOperandLocation (cpu_->registers ());

    if (destOperandLocation.isA<RegisterOperandLocation> ())
        cpu_->registers ()[destOperandLocation] = tmp;
    else
        if (!destOperandLocation.write<u8> (tmp))
            return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, tmp & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !tmp);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _MOVB_H_