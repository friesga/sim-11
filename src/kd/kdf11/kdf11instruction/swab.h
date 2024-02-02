#ifndef _SWAB_H_
#define _SWAB_H_

#include "kdf11instruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/instructions/withfactory.h"

// SWAB - swap bytes
//
// Operation:
//  byte 1/byte 0 <- byte 0/byte 1
//
// Condition Codes:
//  N: set if high-order bit of low-order byte (bit 7) of result is set;
//     cleared otherwise
//  Z: set if low-order byte of result = O; cleared otherwise
//  V: cleared
//  C: cleared
// 
// Exchanges high-order byte and low-order byte of the destination
// word (destination must be a word address).
//
class KDF11Instruction::SWAB : public SingleOperandInstruction, public WithFactory<SWAB>
{
public:
    SWAB (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline KDF11Instruction::SWAB::SWAB (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool KDF11Instruction::SWAB::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return false;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = false});

    if (!writeOperand (operand.value ()))
        return false;

    return true;
}


#endif // _SWAB_H_