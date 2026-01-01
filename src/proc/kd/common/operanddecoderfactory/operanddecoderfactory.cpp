#include "operanddecoderfactory.h"

using std::make_unique;

OperandDecoderFactory::OperandDecoderFactory (CpuData* cpuData,
    Interfaces::CpuController* cpuControl, MMU* mmu)
    :
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}
