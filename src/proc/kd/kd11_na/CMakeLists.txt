set (KD11_NA_CPU_SRCS
    kd11_na.cpp
    cpucontrol/kd11_na_step.cpp
    cpucontrol/kd11_na_cpucontrol.cpp
    cpucontrol/kd11_na_haltmode.cpp
    cpudata/kd11_naregisters/kd11_naregisters.cpp
    pseudommu/pseudommu.cpp
    executor/executor.cpp
    calculate/calculate.cpp)

target_sources (${TARGET} PRIVATE ${KD11_NA_CPU_SRCS})
target_sources (sim-11-test PRIVATE ${KD11_NA_CPU_SRCS})

add_subdirectory (odt)
