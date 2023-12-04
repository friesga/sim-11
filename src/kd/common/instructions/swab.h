#ifndef _SWAB_H_
#define _SWAB_H_

#include "commoninstruction.h"
#include "kd/common/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

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
class CommonInstruction::SWAB : public SingleOperandInstruction, public WithFactory<SWAB>
{
public:
    SWAB (CpuData* cpu, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::SWAB::SWAB (CpuData* cpu, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    SingleOperandInstruction (cpu, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::SWAB::execute ()
{
    CondData<u16> operand;
    if (!readOperand (&operand))
        return false;

    // Swap bytes in the operand and write it to the operand location
    operand = ((operand & 0x00FF) << 8) | ((operand >> 8) & 0xFF);

    if (!writeOperand (operand.value ()))
        return false;

    /*
    setConditionCodeIf_ClearElse (PSW_N, operand & 0x80);
    setConditionCodeIf_ClearElse (PSW_Z, !((u8)operand));
    clearConditionCode (PSW_V);
    clearConditionCode (PSW_C);
    */

    setPSW (ConditionCodes {.N = (bool) (operand & 0x80),
        .Z = (u8) operand == 0,
        .V = false,
        .C = false});


    return true;
}


#endif // _SWAB_H_