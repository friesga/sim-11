#ifndef _OPERANDDECODERFACTORY_
#define _OPERANDDECODERFACTORY_

#include "proc/include/cpudata.h"
#include "proc/include/cpucontroller.h"
#include "proc/common/datapaths/datapaths.h"
#include "proc/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.h"
#include "proc/common/operanddecoders/branchdecoder/branchdecoder.h"
#include "proc/common/operanddecoders/doubleoperanddecoder/doubleoperanddecoder.h"
#include "proc/common/operanddecoders/eisdecoder/eisdecoder.h"
#include "proc/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class OperandDecoderFactory
{
public:
    OperandDecoderFactory (CpuData* cpuData,
        Interfaces::CpuController* cpuController, DataPaths* dataPaths_);
    template <typename T, typename I>
    unique_ptr<T> create (I* instr);

private:
    CpuData* cpuData_;
    Interfaces::CpuController* cpuController_;
    DataPaths* dataPaths_;
};


template <typename T, typename I>
unique_ptr<T> OperandDecoderFactory::create (I* instr)
{
    return make_unique<T> (cpuData_, cpuController_, dataPaths_, instr);
}

#endif //_OPERANDDECODERFACTORY_