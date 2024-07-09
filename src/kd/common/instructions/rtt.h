#ifndef _RTT_H_
#define _RTT_H_


#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// RTT - return from interrupt
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
// Operation is the same as RTI except that it inhibits a trace trap while
// RTI permits trace trap. If new PS has T bit set, trap will occur after
// execution of first instruction after RTT.
//
class RTT : public NoOperandInstruction
{
public:
    RTT (u16 instruction);
};

inline RTT::RTT (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _RTT_H_