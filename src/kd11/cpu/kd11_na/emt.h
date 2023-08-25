#ifndef _EMT_H_
#define _EMT_H_

#include "kd11_na.h"
#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"
#include "withfactory.h"

// EMT - emulator trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (30)
//  PS <- (32)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
// All operation codes from 104000 to 104377 are EMT instructions and may be
// used to transmit information to the emulating routine (e.g., function to be
// performed). The trap vector for EMT is at address 30. The new PC is taken
// from the word at address 30; the new processor status (PS) is taken from the
// word at address 32.
//
class KD11_NA::EMT : public NoOperandInstruction, public WithFactory<EMT>
{
public:
    EMT (CpuData* cpu, u16 instruction);
    CpuData::Trap execute () override;
};

KD11_NA::EMT::EMT (CpuData* cpu, u16 instruction)
    :
    NoOperandInstruction (cpu, instruction)
{}

CpuData::Trap KD11_NA::EMT::execute ()
{
    return CpuData::Trap::EmulatorTrap;
}

#endif // _EMT_H_