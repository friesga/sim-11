set (KT24_SRCS
    kt24.cpp
    unibusmaplogic/unibusmaplogic.cpp)

target_sources (${TARGET} PRIVATE ${KT24_SRCS})
target_sources (sim-11-test PRIVATE ${KT24_SRCS})