#ifndef _OPERANDDECODERFACTORY_
#define _OPERANDDECODERFACTORY_

#include "kd/include/cpudata.h"
#include "kd/include/cpucontrol.h"
#include "kd/include/mmu.h"
#include "kd/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.h"
#include "kd/common/operanddecoders/branchdecoder/branchdecoder.h"
#include "kd/common/operanddecoders/doubleoperanddecoder/doubleoperanddecoder.h"
#include "kd/common/operanddecoders/eisdecoder/eisdecoder.h"
#include "kd/common/operanddecoders/singleoperanddecoder/singleoperanddecoder.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

class OperandDecoderFactory
{
public:
    OperandDecoderFactory (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);
    template <typename T, typename I>
    unique_ptr<T> create (I* instr);

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;
};


template <typename T, typename I>
unique_ptr<T> OperandDecoderFactory::create (I* instr)
{
    return make_unique<T> (cpuData_, cpuControl_, mmu_, instr);
}

#endif //_OPERANDDECODERFACTORY_