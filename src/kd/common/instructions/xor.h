#ifndef _XOR_H_
#define _XOR_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/eisinstruction/eisinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
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
    XOR (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::XOR::XOR (CpuData* cpu, u16 instruction)
    :
    EisInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::XOR::execute ()
{
    u16 regNr = getRegisterNr ();
    CpuData::GeneralRegisters& registers = cpu_->registers ();

    u16 source = registers[regNr];

    CondData<u16> destination;
    if (!readOperand (&destination))
        return CpuData::Trap::BusError;

    u16 result = source ^ destination;

    if (!writeOperand (result))
        return CpuData::Trap::BusError;

    setConditionCodeIf_ClearElse (PSW_N, result & 0x8000);
    setConditionCodeIf_ClearElse (PSW_Z, !result);
    clearConditionCode (PSW_V);

    return CpuData::Trap::None;
}

#endif // _XOR_H_