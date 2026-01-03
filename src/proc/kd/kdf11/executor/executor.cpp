#include "executor.h"

KDF11_Executor::KDF11_Executor (CpuData* cpuData,
    Interfaces::CpuController* cpuController, MMU* mmu)
    :
    commonExecutor_ (cpuData, cpuController, mmu),
    cpuData_ {cpuData},
    cpuController_ {cpuController},
    mmu_ {mmu}
{}
