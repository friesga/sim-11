set (ABSTRACTBUSDEVICE_SRCS
    writebyte.cpp)

target_sources (${TARGET} PRIVATE ${ABSTRACTBUSDEVICE_SRCS})
target_sources (sim-11-test PRIVATE ${ABSTRACTBUSDEVICE_SRCS})