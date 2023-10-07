#ifndef _MARK_H_
#define _MARK_H_

#include "commoninstruction.h"
#include "kd/kd11_na/cpu/markinstruction/markinstruction.h"
#include "kd/kd11_na/cpu/cpudata.h"
#include "kd/kd11_na/cpu/operandlocation/operandlocation.h"
#include "withfactory.h"

// MARK - mark
//
// Operation:
//  SP <- updated PC + 2 + 2n n = number of parameters
//  PC <- R5
//  R5 <- (SP)^
//
// Condition Codes: unaffected
//
// MARK n causes:
//  (1) the stack pointer to be adjusted to point to the old R5 value;
//  (2) the value now in R5 (the old PC) to be placed in the PC; and
//  (3) contents of the old R5 to be popped into R5 thus completing the
//      return from subroutine.
//
class CommonInstruction::MARK : public MarkInstruction, public WithFactory<MARK>
{
public:
    MARK (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

CommonInstruction::MARK::MARK (CpuData* cpu, u16 instruction)
    :
    MarkInstruction (cpu, instruction)
{}

CpuData::Trap CommonInstruction::MARK::execute ()
{
    CpuData::GeneralRegisters& registers = cpu_->registers ();

    registers[6] = registers[7] + 2 * numberOfParameters ();
    registers[7] = registers[5];
    registers[5] = cpu_->fetchWord (registers[6]);
    registers[6] += 2;

    return CpuData::Trap::None;
}

#endif // _MARK_H_