#ifndef _MTPS_H_
#define _MTPS_H_

#include "kd11_na.h"
#include "../singleoperandinstruction/singleoperandinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// MTPS - Move byte to Processor Status Word
//
// Operation:
//  PSW <- (src)
//
// Condition Codes:
//  Set according to effective SRC operand bits 0-3
//
// The 8 bits of the effective operand replaces the current contents of the
// PSW. The source operand address is treated as a byte address. Note that the
// T bit (PSW bit 4) cannot be set with this instruction. The SRC operand
// remains unchanged. This instruction can be used to change the priority bit
// (PSW bit 7) in the PSW
//
class KD11_NAInstruction::MTPS : public SingleOperandInstruction, public WithFactory<MTPS>
{
public:
    MTPS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::MTPS::MTPS (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::MTPS::execute ()
{
    CondData<u8> newValue;
    if (!readOperand (&newValue))
        return CpuData::Trap::BusError;

    // Allow bits 5/6/7 to be set and cleared
    cpu_->psw () = (cpu_->psw () & PSW_T) | (newValue & ~PSW_T);

    return CpuData::Trap::None;
}

#endif // _MTPS_H_