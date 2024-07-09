#ifndef _HALT_H_
#define _HALT_H_


#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// HALT - halt
// 
// Condition Codes: not affected
// 
// Causes the processor to leave RUN mode. The PC points to the next
// instruction to be executed. The processor goes into the HALT mode.
// The console mode of operation is enabled.
//
class HALT : public NoOperandInstruction
{
public:
    HALT (u16 instruction);
};

inline HALT::HALT (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _HALT_H_