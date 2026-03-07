set (KD11_NA_CPU_SRCS
    kd11_na.cpp
    executor/executor.cpp
    calculator/calculator.cpp)

target_sources (${TARGET} PRIVATE ${KD11_NA_CPU_SRCS})
target_sources (sim-11-test PRIVATE ${KD11_NA_CPU_SRCS})

add_subdirectory (odt)
