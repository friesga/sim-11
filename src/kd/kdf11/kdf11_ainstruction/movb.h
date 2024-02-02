#ifndef _MOVB_H_
#define _MOVB_H_

#include "kdf11_ainstruction.h"
#include "kd/kdf11/kd11doubleoperandinstruction/kd11doubleoperandinstruction.h"
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
class KDF11_AInstruction::MOVB : public KD11DoubleOperandInstruction, public WithFactory<MOVB>
{
public:
    MOVB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::MOVB::MOVB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    KD11DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11_AInstruction::MOVB::execute ()
{
    CondData<u8> source;

    if (!readSourceOperand (&source))
        return false;

    // Make the source a signed eight bit value 
    s8 tmp = (s8) source;

    setPSW (ConditionCodes ({.N = (bool) (tmp & 0x80),
        .Z = tmp == 0,
        .V = false}));

    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value s8 is directly written into
    // the register, causing sign extension in the register.
    if (destinationOperandLocation_.isA<RegisterOperandLocation> ())
        cpuData_->registers ()[destinationOperandLocation_] = tmp;
    else
        if (!destinationOperandLocation_.write<u8> (tmp))
            return false;

    return true;
}

#endif // _MOVB_H_