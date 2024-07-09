#ifndef _UNUSED_H_
#define _UNUSED_H_


#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "trace/trace.h"


// Unused/reserved instruction
//
// Reserved instructions trap to vector address 10.
//
class Unused : public NoOperandInstruction
{
public:
    Unused (u16 instruction);
};

// The two function members are defined inline as this header file is
// included in both opcodetables.cpp and kd11_na.cpp.
inline Unused::Unused (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _UNUSED_H_