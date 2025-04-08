set (PDP11_SRCS
    pdp11.cpp
    configure.cpp
    installmodules.cpp
    run.cpp)

target_sources (${TARGET} PRIVATE ${PDP11_SRCS})
target_sources (sim-11-test PRIVATE ${PDP11_SRCS})