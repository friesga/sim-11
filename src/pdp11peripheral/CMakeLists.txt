set (PDP11PERIPHERAL_SRCS
    pdp11peripheral.cpp)

target_sources (${TARGET} PRIVATE ${PDP11PERIPHERAL_SRCS})
target_sources (sim-11-test PRIVATE ${PDP11PERIPHERAL_SRCS})