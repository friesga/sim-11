set (SIGNAL_SRCS
    signal.cpp)

target_sources (${TARGET} PRIVATE ${SIGNAL_SRCS})
target_sources (sim-11-test PRIVATE ${SIGNAL_SRCS})