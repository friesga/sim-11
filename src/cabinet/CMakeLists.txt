set (CABINET_SRCS
    cabinet.cpp)

target_sources (${TARGET} PRIVATE ${CABINET_SRCS})
target_sources (sim-11-test PRIVATE ${CABINET_SRCS})