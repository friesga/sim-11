#ifndef _SOB_H_
#define _SOB_H_

#include "kd11_nainstruction.h"
#include "../sobinstruction/sobinstruction.h"
#include "../cpudata.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// SOB - subtract one and branch (if != 0)
//
// Operation:
//  (R) <- (R) - 1; if this result != 0 then PC <- PC - (2 * offset)
//  if (R) = O; PC <- PC
//
// Condition Codes: unaffected
//
// The register is decremented. If it is not equal to 0, twice the offset is
// subtracted from the PC (now pointing to the following word). The offset is
// interpreted as a sixbit positive number.
//
class KD11_NAInstruction::SOB : public SobInstruction, public WithFactory<SOB>
{
public:
    SOB (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NAInstruction::SOB::SOB (CpuData* cpu, u16 instruction)
    :
    SobInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NAInstruction::SOB::execute ()
{
    u16 regNr = getRegisterNr ();
    u16 offset = getOffset ();

    CpuData::GeneralRegisters& reg = cpu_->registers ();
    reg[regNr]--;
    if (reg[regNr])
        reg[7] -= 2 * offset;

    return CpuData::Trap::None;
}

#endif // _SOB_H_