#include "executor.h"

KA11_Executor::KA11_Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    commonExecutor_ {cpuData, cpuControl, mmu},
    cpuData_ {cpuData}
{}