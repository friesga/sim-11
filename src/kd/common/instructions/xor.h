#ifndef _XOR_H_
#define _XOR_H_

#include "commoninstruction.h"
#include "kd/common/eisinstruction/eisinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "withfactory.h"

// XOR - exclusive OR
//
// Operation:
//  (dst) <- R v (dst)
//
// Condition Codes:
//  N: set if the result <0: cleared otherwise
//  Z: set if result = O: cleared otherwise
//  V: cleared
//  C: unaffected
//
// The exclusive OR of the register and destination operand is stored in the
// destination address. Contents of register are unaffected.
//
class CommonInstruction::XOR : public EisInstruction, public WithFactory<XOR>
{
public:
    XOR (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu, u16 instruction);
    bool execute () override;
};

inline CommonInstruction::XOR::XOR (CpuData* cpuData, CpuControl* cpuControl,
        MMU* mmu, u16 instruction)
    :
    EisInstruction (cpuData, cpuControl, mmu, instruction)
{}

inline bool CommonInstruction::XOR::execute ()
{
    u16 regNr = getRegisterNr ();
    GeneralRegisters& registers = cpuData_->registers ();

    u16 source = registers[regNr];

    CondData<u16> destination;
    if (!readOperand (&destination))
        return false;

    u16 result = source ^ destination;

    if (!writeOperand (result))
        return false;

    setPSW (ConditionCodes {.N = (bool) (result & 0x8000),
        .Z = result == 0,
        .V = false});

    return true;
}

#endif // _XOR_H_