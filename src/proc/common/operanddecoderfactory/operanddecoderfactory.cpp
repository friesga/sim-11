#include "operanddecoderfactory.h"

using std::make_unique;

OperandDecoderFactory::OperandDecoderFactory (CpuData* cpuData,
    Interfaces::CpuController* cpuController, DataPaths* dataPaths)
    :
    cpuData_ {cpuData},
    cpuController_ {cpuController},
    dataPaths_ {dataPaths}
{}
