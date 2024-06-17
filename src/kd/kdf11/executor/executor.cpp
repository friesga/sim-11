#include "executor.h"

KDF11_A::Executor::Executor::Executor (CpuData* cpuData, CpuControl* cpuControl, MMU* mmu)
    :
    commonExecutor (cpuData, cpuControl, mmu),
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}
