set (LOGGER_SRCS
    logger.cpp
    data.cpp)

target_sources (${TARGET} PRIVATE ${LOGGER_SRCS})
target_sources (sim-11-test PRIVATE ${LOGGER_SRCS})