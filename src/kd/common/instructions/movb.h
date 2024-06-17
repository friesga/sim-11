#ifndef _MOVB_H_
#define _MOVB_H_


#include "kd/common/instructionformats/doubleoperandinstruction/doubleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


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
class MOVB : public DoubleOperandInstruction
{
public:
    MOVB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool writeDestinationOperands8 (s8 operand);
    bool execute () override;
};

inline MOVB::MOVB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    DoubleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool MOVB::writeDestinationOperands8 (s8 operand)
{
    // If the destination mode is 0 (Register) the regular operand processing
    // is bypassed and the signed eight bit value s8 is directly written into
    // the register, causing sign extension in the register.
    if (destinationOperandLocation_.isA<RegisterOperandLocation> ())
        cpuData_->registers ()[destinationOperandLocation_] = operand;
    else
        if (!destinationOperandLocation_.write<u8> (operand))
            return false;

    return true;
}

// ToDo: To be removed
inline bool MOVB::execute ()
{
    throw "Should not happen";
}

#endif // _MOVB_H_