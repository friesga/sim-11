#ifndef _MFPS_H_
#define _MFPS_H_

#include "kdf11_ainstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// MFPS - Move byte from Processor Status Word
//
// Operation:
//  (dst) <- PSW
//  dst lower 8 bits
//
// Condition Codes:
//  N = set if PSW bit 7 = 1; cleared otherwise
//  Z = set if PS <0:7> = 0; cleared otherwise�
//  V = cleared
//  C = not affected
// 
// The 8 bit contents of the PS are moved to the effective destination. If
// destination is mode 0, PS bit 7 is sign extended through upper byte of the
// register. The destination operand address is treated as a byte address.
// 
// This is a KDF11-A specific version of the MFPS instruction. This version
// sets the condition codes before the destination operand is written. This
// behaviour satisfies the JKDBD0 diagnostic test 374, which executes the
// odd instruction MFPS 177776, i.e. writes the lower byte of the PSW to
// itself.
//
class KDF11_AInstruction::MFPS : public SingleOperandInstruction, public WithFactory<MFPS>
{
public:
    MFPS (CpuData* cpu, u16 instruction);
    bool execute () override;
};

inline KDF11_AInstruction::MFPS::MFPS (CpuData* cpu, u16 instruction)
    :
    SingleOperandInstruction (cpu, instruction)
{}

inline bool KDF11_AInstruction::MFPS::execute ()
{
    u8 contents = (u8) cpu_->psw ();
    operandLocation_ = getOperandLocation (cpu_->registers ());

    setPSW (ConditionCodes {.N = (bool) (contents & 0x80),
        .Z = (contents & 0xFF) == 0,
        .V = false});

    if (operandLocation_.isA<RegisterOperandLocation> ())
    {
        // If destination is mode 0 (Register), the regular operand processing
        // is bypassed and PS bit 7 is sign extended through the upper byte of
        // the register.
        cpu_->registers ()[operandLocation_] = (s8) contents;
    }
    else
    {
        if (!writeOperand (contents))
            return false;
    }
    
    return true;
}

#endif // _MFPS_H_