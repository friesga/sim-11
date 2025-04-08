set (FLOAT_SRCS
    float.cpp
    ieeetopdp11.cpp
    pdp11toieee.cpp)

target_sources (${TARGET} PRIVATE ${FLOAT_SRCS})
target_sources (sim-11-test PRIVATE ${FLOAT_SRCS})