#include "executor.h"

KA11_Executor::KA11_Executor (CpuData* cpuData, Interfaces::CpuController* cpuControl, MMU* mmu)
    :
    commonExecutor_ {cpuData, cpuControl, mmu},
    cpuData_ {cpuData}
{}