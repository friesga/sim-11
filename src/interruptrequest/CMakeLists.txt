set (INTERRUPTREQUEST_SRCS
    interruptrequest.cpp)

target_sources (${TARGET} PRIVATE ${INTERRUPTREQUEST_SRCS})
target_sources (sim-11-test PRIVATE ${INTERRUPTREQUEST_SRCS})