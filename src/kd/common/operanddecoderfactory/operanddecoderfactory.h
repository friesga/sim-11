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

class OperandDecoderFactory
{
public:
    OperandDecoderFactory (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu);
    unique_ptr<BranchDecoder> createBranchDecoder (BranchInstruction* instr);
    unique_ptr<DoubleOperandDecoder> createDoubleOperandDecoder (DoubleOperandInstruction* instr);
    unique_ptr<EisDecoder> createEisDecoder (EisInstruction* instr);
    unique_ptr<SingleOperandDecoder> createSingleOperandDecoder (SingleOperandInstruction* instr);

private:
    CpuData* cpuData_;
    CpuControl* cpuControl_;
    MMU* mmu_;
};

#endif //_OPERANDDECODERFACTORY_