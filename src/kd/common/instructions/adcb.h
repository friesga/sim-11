#ifndef _ADCB_H_
#define _ADCB_H_


#include "kd/common/instructionformats/singleoperandinstruction/singleoperandinstruction.h"
#include "kd/include/cpudata.h"
#include "kd/common/operandlocation/operandlocation.h"


// ADCB - add carry byte
//
// Operation:
//  refer to ADC
// 
// Condition Codes:
//  refer to ADC
//
class ADCB : public SingleOperandInstruction
{
public:
    ADCB (u16 instruction);
};

inline ADCB::ADCB (u16 instruction)
    :
    SingleOperandInstruction (instruction)
{}


#endif // _ADCB_H_