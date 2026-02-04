#include "executor.h"

KA11_Executor::KA11_Executor (CpuData* cpuData,
    Interfaces::CpuController* cpuController, DataPaths* dataPaths)
    :
    commonExecutor_ {cpuData, cpuController, dataPaths},
    cpuData_ {cpuData}
{}