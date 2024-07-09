#ifndef _RESET_H_
#define _RESET_H_


#include "kd/common/instructionformats/nooperandinstruction/nooperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/common/operandlocation/operandlocation.h"


// RESET - reset external bus
//
// Condition Codes: not affected
//
// Sends INIT on the BUS for 10 micro seconds. All devices on the BUS are
// reset to their state at power-up. The processor remains in an idle state
// for 90 micro seconds following issuance of INIT.
//
class RESET : public NoOperandInstruction
{
public:
    RESET (u16 instruction);
};

inline RESET::RESET (u16 instruction)
    :
    NoOperandInstruction (instruction)
{}

#endif // _RESET_H_