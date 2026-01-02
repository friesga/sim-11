#ifndef _IOT_H_
#define _IOT_H_


#include "proc/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "proc/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/common/operandlocation/operandlocation.h"


// IOT - input/output trap
//
// Operation:
//  v(SP) <- PS
//  v(SP) <- PC
//  PC <- (20)
//  PS <- (22)
//
// Condition Codes:
//  N: loaded from trap vector
//  Z: loaded from trap vector
//  V: loaded from trap vector
//  C: loaded from trap vector
//
class IOT : public NoOperandInstruction
{
public:
    IOT (u16 instruction);
};

inline IOT::IOT (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _IOT_H_