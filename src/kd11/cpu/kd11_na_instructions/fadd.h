#ifndef _FADD_H_
#define _FADD_H_

#include "../fisinstruction/fisinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"

// FADD - floating add
//
// Operation:
//  [(R)+4, (R)+6] <- [(R)+4, (R)+6]+[(R),(R)+2],
//  if result >= 2^-128; else [(R)+4, (R)+6] <- 0
//
// Condition Codes:
//  N: set if result < O; cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: cleared
//
// Adds the A argument to the B argument and stores the result in the
// A Argument position on the stack. General register R is used as the stack
// pointer for the operation.
// 
// A <- A + B
//
namespace KD11_NA
{
    class FADD : public FISInstruction
    {
    public:
        FADD (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;

    private:
        CpuData* cpu_;
        u16 instruction_;
    };

    FADD::FADD (CpuData* cpu, u16 instruction)
        :
        FISInstruction (cpu, instruction),
        cpu_ {cpu},
        instruction_ {instruction}
    {}

    CpuData::Trap FADD::execute ()
    {
        FISInstruction fisInstruction (cpu_, instruction_);

        return executeFISinstruction (fisInstruction.getRegister (),
            [] (Float f1, Float f2) {return true;},
            [] (Float f1, Float f2) {return f1 + f2;});
    }
}

#endif // _FADD_H_