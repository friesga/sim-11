#ifndef _BASEOPERANDDECODER_H_
#define _BASEOPERANDDECODER_H_

#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"
#include "kd/common/operandlocation/operandlocation.h"
#include "kd/common/operand/operand.h"

class BaseOperandDecoder
{
public:
    BaseOperandDecoder (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);
    
protected:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;

    OperandLocation decodeOperand (u16 opCode, Operand operand,
        GeneralRegisters &reg);

private:
    bool isByteInstruction (u16 opCode);
};

#endif // !_BASEOPERANDDECODER_H_
