#ifndef _MFPT_H_
#define _MFPT_H_

#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// MFPT - Move from processor type
// //
// Operation:
//  R0 <- 000003 
//
// Condition Codes: 
//  N: unaffected 
//  Z: unaffected 
//  V: unaffected 
//  C: unaffected 
//
// A unique number assigned to each PDP-11 processor model is loaded into
// general register R0. The KDF11-AA processor number is 000003 and can be
// used to indicate which processor a program is being executed on. LSI-11 and
// LSI-11/2 processors treat this op code as a reserved instruction trap. 
//
class MFPT : public NoOperandInstruction
{
public:
    MFPT (u16 instruction);
};

inline MFPT::MFPT (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _MFPT_H_