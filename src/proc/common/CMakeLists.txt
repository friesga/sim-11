set (PROC_COMMON_SRCS
    cpudata/singularregisterset/singularregisterset.cpp
    basemachinestate/basemachinestate.cpp
    basemachinestate/signalreceivers.cpp
    basemachinestate/statemachine.cpp
    executor/executor.cpp
    pseudommu/pseudommu.cpp
    pseudo_haltmode/pseudo_haltmode.cpp)

target_sources (${TARGET} PRIVATE ${PROC_COMMON_SRCS})
target_sources (sim-11-test PRIVATE ${PROC_COMMON_SRCS})