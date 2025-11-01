#ifndef _BASEOPERANDDECODER_H_
#define _BASEOPERANDDECODER_H_

#include "proc/kd/include/cpudata.h"
#include "proc/kd/include/cpucontrol.h"
#include "proc/kd/include/mmu.h"
#include "proc/kd/common/operandlocation/operandlocation.h"
#include "proc/kd/common/operand/operand.h"

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
