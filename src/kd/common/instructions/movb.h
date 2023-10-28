#ifndef _MOVB_H_
#define _MOVB_H_

#include "commoninstruction.h"
#include "kd/common/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
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
    bool execute () override;
};

inline CommonInstruction::MOVB::MOVB (CpuData* cpu, u16 instruction)
    :
    DoubleOperandInstruction (cpu, instruction)
{}

inline bool CommonInstruction::MOVB::execute ()
{
    CondData<u8> source;

    if (!readSourceOperand (&source))
        return false;

    s8 tmp = (s8) source;

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value u8 is directly written into
    // the register, causing sign extension in the register.
    destinationOperandLocation_ = 
        getDestinationOperandLocation (cpu_->registers ());

    if (destinationOperandLocation_.isA<RegisterOperandLocation> ())
        cpu_->registers ()[destinationOperandLocation_] = tmp;
    else
        if (!destinationOperandLocation_.write<u8> (tmp))
            return false;

    setPSW (ConditionCodes ({.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false}));

    return true;
}

#endif // _MOVB_H_