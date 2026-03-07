#include "operanddecoderfactory.h"

using std::make_unique;

OperandDecoderFactory::OperandDecoderFactory (CpuData* cpuData,
    Interfaces::CpuController* cpuController, MMU* mmu)
    :
    cpuData_ {cpuData},
    cpuController_ {cpuController},
    mmu_ {mmu}
{}
