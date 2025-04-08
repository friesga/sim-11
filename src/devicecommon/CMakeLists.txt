set (DEVICECOMMON_SRCS
    registerhandler/registerhandler.cpp)

target_sources (${TARGET} PRIVATE ${DEVICECOMMON_SRCS})
target_sources (sim-11-test PRIVATE ${DEVICECOMMON_SRCS})