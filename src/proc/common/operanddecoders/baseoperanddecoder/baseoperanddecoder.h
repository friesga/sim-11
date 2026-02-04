#ifndef _BASEOPERANDDECODER_H_
#define _BASEOPERANDDECODER_H_

#include "proc/include/cpudata.h"
#include "proc/include/cpucontroller.h"
#include "proc/common/datapaths/datapaths.h"
#include "proc/common/operandlocation/operandlocation.h"
#include "proc/common/operand/operand.h"

class BaseOperandDecoder
{
public:
    BaseOperandDecoder (CpuData* cpuData,
        Interfaces::CpuController* cpuController, DataPaths* dataPaths);
    
protected:
    CpuData* cpuData_;
    Interfaces::CpuController* cpuController_;
    DataPaths* dataPaths_;

    OperandLocation decodeOperand (u16 opCode, Operand operand,
        GeneralRegisters &reg);

private:
    bool isByteInstruction (u16 opCode);
};

#endif // !_BASEOPERANDDECODER_H_
