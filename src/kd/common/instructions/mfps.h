#ifndef _MFPS_H_
#define _MFPS_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// MFPS - Move byte from Processor Status Word
//
// Operation:
//  (dst) < PSW
//  dst lower 8 bits
//
// Condition Codes:
//  N = set if PSW bit 7 = 1; cleared otherwise
//  Z = set if PS <0:7> = O; cleared otherwise·
//  V = cleared
//  C = not affected
// 
// The 8 bit contents of the PS are moved to the effective destination. If
// destination is mode 0, PS bit 7 is sign extended through upper byte of the
// register. The destination operand address is treated as a byte address.
//
class CommonInstruction::MFPS : public SingleOperandInstruction, public WithFactory<MFPS>
{
public:
    MFPS (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

inline CommonInstruction::MFPS::MFPS (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline CpuData::Trap CommonInstruction::MFPS::execute ()
{
    u8 contents = (u8) cpu_->psw ();
    operandLocation_ = getOperandLocation (cpu_->registers ());

    if (operandLocation_.isA<RegisterOperandLocation> ())
    {
        // If destination is mode 0 (Register), the regular operand processing
        // is bypassed and PS bit 7 is sign extended through the upper byte of
        // the register.
        cpu_->registers ()[operandLocation_] = (s8)cpu_->psw ();
    }
    else
    {
        if (!writeOperand (contents))
            return CpuData::Trap::BusError;
    }
    setConditionCodeIf_ClearElse (PSW_N, contents & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !(contents & 0xFF));
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _MFPS_H_