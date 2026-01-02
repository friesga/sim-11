#ifndef _OPERANDDECODERFACTORY_
#define _OPERANDDECODERFACTORY_

#include "proc/kd/include/cpudata.h"
#include "proc/include/cpucontrol.h"
#include "proc/kd/include/mmu.h"
#include "proc/kd/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.h"
#include "proc/kd/common/operanddecoders/branchdecoder/branchdecoder.h"
#include "proc/kd/common/operanddecoders/doubleoperanddecoder/doubleoperanddecoder.h"
#include "proc/kd/common/operanddecoders/eisdecoder/eisdecoder.h"
#include "proc/kd/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class OperandDecoderFactory
{
public:
    OperandDecoderFactory (CpuData* cpuData, Interfaces::CpuController* cpuControl, MMU* mmu);
    template <typename T, typename I>
    unique_ptr<T> create (I* instr);

private:
    CpuData* cpuData_;
    Interfaces::CpuController* cpuControl_;
    MMU* mmu_;
};


template <typename T, typename I>
unique_ptr<T> OperandDecoderFactory::create (I* instr)
{
    return make_unique<T> (cpuData_, cpuControl_, mmu_, instr);
}

#endif //_OPERANDDECODERFACTORY_