#ifndef _TRAP_H_
#define _TRAP_H_

#include "../nooperandinstruction/nooperandinstruction.h"
#include "../cpudata.h"
#include "../cpucontrol.h"
#include "../operandlocation/operandlocation.h"

// TRAP - trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (34)
//  PS <- (36)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector 
//
// Operation codes from 104400 to 104777 are TRAP instructions. TRAPs and EMTs
// are identical in operation, except that the trap vector for TRAP is at
// address 34.
//
namespace KD11_NA
{
    class TRAP : public NoOperandInstruction
    {
    public:
        TRAP (CpuData* cpu, u16 instruction);
        CpuData::Trap execute () override;
    };

    TRAP::TRAP (CpuData* cpu, u16 instruction)
        :
        NoOperandInstruction (cpu, instruction)
    {}

    CpuData::Trap TRAP::execute ()
    {
        return CpuData::Trap::TRP;
    }
}

#endif // _TRAP_H_