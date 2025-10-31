#ifndef _SCC_H_
#define _SCC_H_


#include "proc/kd/common/instructionformats/ccinstruction/ccinstruction.h"
#include "proc/kd/include/cpudata.h"
#include "proc/kd/common/operandlocation/operandlocation.h"


//
// Set condition code bits. Selectable combinations of these bits may
// be set together. Condition code bits in the PSW corresponding to bits
// in the condition code operator (bits O-3) are set.
//  
class SCC : public CcInstruction
{
public:
    SCC (u16 instruction);
};

inline SCC::SCC (u16 instruction)
    :
    CcInstruction (instruction)
{}

#endif // _CCC_H_