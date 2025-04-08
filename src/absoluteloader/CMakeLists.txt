set (ABSOLUTELOADER_SRCS
    absoluteloader.cpp)

target_sources (${TARGET} PRIVATE ${ABSOLUTELOADER_SRCS})
target_sources (sim-11-test PRIVATE ${ABSOLUTELOADER_SRCS})