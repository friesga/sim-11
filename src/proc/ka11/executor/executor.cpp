#include "executor.h"

KA11_Executor::KA11_Executor (CpuData* cpuData,
    Interfaces::CpuController* cpuController, MMU* mmu)
    :
    commonExecutor_ {cpuData, cpuController, mmu},
    cpuData_ {cpuData}
{}