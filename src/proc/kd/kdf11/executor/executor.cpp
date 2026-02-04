#include "executor.h"

KDF11_Executor::KDF11_Executor (CpuData* cpuData,
    Interfaces::CpuController* cpuController, DataPaths* dataPaths)
    :
    commonExecutor_ (cpuData, cpuController, dataPaths),
    cpuData_ {cpuData},
    cpuController_ {cpuController},
    dataPaths_ {dataPaths}
{}
