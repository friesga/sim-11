#ifndef _BPT_H_
#define _BPT_H_


#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// BPT - breakpoint trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (14)
//  PS <- (16)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
class BPT : public NoOperandInstruction
{
public:
    BPT (u16 instruction);
};

inline BPT::BPT (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _BPT_H_