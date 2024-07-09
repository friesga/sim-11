#include "operanddecoderfactory.h"

using std::make_unique;

OperandDecoderFactory::OperandDecoderFactory (CpuData* cpuData,
    CpuControl* cpuControl, MMU* mmu)
    :
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}

unique_ptr<BranchDecoder> 
    OperandDecoderFactory::createBranchDecoder (BranchInstruction* instr)
{
    return make_unique<BranchDecoder> (cpuData_, cpuControl_, mmu_, instr);
}

unique_ptr<DoubleOperandDecoder>
    OperandDecoderFactory::createDoubleOperandDecoder (DoubleOperandInstruction* instr)
{
    return make_unique<DoubleOperandDecoder> (cpuData_, cpuControl_, mmu_, instr);
}

unique_ptr<EisDecoder>
    OperandDecoderFactory::createEisDecoder (EisInstruction* instr)
{
    return make_unique<EisDecoder> (cpuData_, cpuControl_, mmu_, instr);
}

unique_ptr<SingleOperandDecoder> 
    OperandDecoderFactory::createSingleOperandDecoder (SingleOperandInstruction* instr)
{
    return make_unique<SingleOperandDecoder> (cpuData_, cpuControl_, mmu_, instr);
}
