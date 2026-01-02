#ifndef _RTI_H_
#define _RTI_H_


#include "proc/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/common/operandlocation/operandlocation.h"


// RTI - return from interrupt
//
// Operation:
//  PC <- (SP)^
//  PS <- (SP)^
//
// Conditions Codes:
//  N: loaded from processor stack
//  Z: loaded from processor stack
//  V: loaded from processor stack
//  C: loaded from processor stack
//
// Used to exit from an interrupt or TRAP service routine. The PC and PS are
// restored (popped) from-the processor stack. If a trace trap is pending, the
// first instruction after RTI will not be executed prior to the next T traps.
//
class RTI : public NoOperandInstruction
{
public:
    RTI (u16 instruction);
};

inline RTI::RTI (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _RTI_H_