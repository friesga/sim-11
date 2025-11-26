set (PROC_COMMON_SRCS
    cpudata/singularregisterset/singularregisterset.cpp)

target_sources (${TARGET} PRIVATE ${PROC_COMMON_SRCS})
target_sources (sim-11-test PRIVATE ${PROC_COMMON_SRCS})