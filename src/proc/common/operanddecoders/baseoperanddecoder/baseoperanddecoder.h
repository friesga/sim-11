#ifndef _BASEOPERANDDECODER_H_
#define _BASEOPERANDDECODER_H_

#include "proc/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/include/mmu.h"
#include "proc/common/operandlocation/operandlocation.h"
#include "proc/common/operand/operand.h"

class BaseOperandDecoder
{
public:
    BaseOperandDecoder (CpuData* cpuData, Interfaces::CpuController* cpuControl, MMU* mmu);
    
protected:
    CpuData* cpuData_;
    Interfaces::CpuController* cpuControl_;
    MMU* mmu_;

    OperandLocation decodeOperand (u16 opCode, Operand operand,
        GeneralRegisters &reg);

private:
    bool isByteInstruction (u16 opCode);
};

#endif // !_BASEOPERANDDECODER_H_
