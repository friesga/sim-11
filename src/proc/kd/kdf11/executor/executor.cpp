#include "executor.h"

KDF11_Executor::KDF11_Executor (CpuData* cpuData, Interfaces::CpuController* cpuControl, MMU* mmu)
    :
    commonExecutor_ (cpuData, cpuControl, mmu),
    cpuData_ {cpuData},
    cpuControl_ {cpuControl},
    mmu_ {mmu}
{}
