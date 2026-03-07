#ifndef _TRAP_H_
#define _TRAP_H_


#include "proc/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/include/cpucontroller.h"
#include "proc/common/operandlocation/operandlocation.h"


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
class TRAP : public NoOperandInstruction
{
public:
    TRAP (u16 instruction);
};

inline TRAP::TRAP (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _TRAP_H_